/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     cfgMgr.c
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/11/22
 * @defgroup cfgMgr
 * @ingroup  fpgaRemoteUpdate  
 * @brief    fpga remote update protocol v1.01
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/11/22    Chenxu        None              File Create
 ****************************************************************************** 
 */
#include <fpgaRemoteUpdate.h>
#include <string.h>
#include <trace.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <fpgaCtrl.h>
#include <stdlib.h>

#define RECV_TIMEOUT_SEC        60

void printbuffer(char *fmt, u8 *buf, u16 len)
{
    int i;

    printf ("%s", fmt);
    for (i = 0; (i < len) && (i < 30); i++)
        printf("%02x ", buf[i]);
    printf ("\n");
}

static u8 calcFCS (u8 *data, u16 len)
{
    u16 i;
    u8 fcs = 0;

    for (i = 0; i < len; i++)
        fcs += data[i];

    return fcs;
}

static int fpgaRmtUdtPack (u8 fc, u8 *payload, u16 pLen, u8 *sBuf)
{
    fgpaRemoteUpdateHeader *h = (fgpaRemoteUpdateHeader *)sBuf;
    u8                     *p;

    h->sd2_0     = SD2_CONST;
    h->le_byte0  = ((int)(pLen + 3)) % 256;
    h->le_byte1  = ((int)(pLen + 3)) / 256;
    h->ler_byte0 = ((int)(pLen + 3)) % 256;
    h->ler_byte1 = ((int)(pLen + 3)) / 256;
    h->sd2_1     = SD2_CONST;
    h->da        = DA_CONST;
    h->sa        = SA_CONST;
    h->fc        = fc;

    p = sBuf + sizeof(fgpaRemoteUpdateHeader);

    if (pLen != 0)
    {
        if (payload != NULL)
        {
            memcpy(p, payload, pLen);
        }
        else
            return -1;
    }

    p[pLen] = calcFCS(&h->da, pLen + 3);

    p[pLen + 1] = ED_CONST;

    return (sizeof(fgpaRemoteUpdateHeader) + pLen + 2);
}

static int fpgaRmtUdtUnPack (u8 *frame, u16 fLen, fgpaRemoteUpdateHeader *head, u8 *payload)
{
    u8                     fcs;
    fgpaRemoteUpdateHeader *h = (fgpaRemoteUpdateHeader *)frame;  
    u16                    le, ler;
    
    if ((fLen < FRAME_LEN_MIN) || (fLen > FRAME_LEN_MAX))
        return FRAME_LEN_INVALID;

    if (h->sd2_0 != SD2_CONST)
        return FRAME_SD2_0_INVALID;

    le = h->le_byte0 + (h->le_byte1 << 8);
    ler = h->ler_byte0 + (h->ler_byte1 << 8);
    if ((le != ler) || (le < LE_MIN) || (le > LE_MAX))
        return FRAME_LE_INVALID;

    if (h->sd2_1 != SD2_CONST)
        return FRAME_SD2_1_INVALID;

    if ((h->fc != FC_UPDATE_START_RESP)
        && (h->fc != FC_TRANS_RESP)
        && (h->fc != FC_UPDATE_END_RESP)
        && (h->fc != FC_QUERY_VERSION_RESP)
        && (h->fc != FC_VERSION_SW_RESP)
        && (h->fc != FC_ACCESS_REG_RESP))
        return FRAME_FC_INVALID;

    fcs = frame[6 + le];

    if (fcs != calcFCS(&h->da, le))
        return FRAME_FCS_INVALID;

    if (frame[6 + le + 1] != ED_CONST)
        return FRAME_ED_INVALID;

    memcpy (head, h, sizeof(fgpaRemoteUpdateHeader));
    memcpy (payload, frame + sizeof(fgpaRemoteUpdateHeader), le - 3);

    return FRAME_OK;
}

static int fpgaRmtUdtRequest(int sockfd, u8 fc, u8 *sendPayload, u16 pLen)
{
    u8                      sendFrame[1600], recvFrame[1600];
    struct sockaddr_in      server;
    int                     sendLen, recvLen;
    int                     fromLen = sizeof(struct sockaddr_in);
    int                     retrytimes = 0;
    u8                      data[8];
    fgpaRemoteUpdateHeader  head;
    frameStatus             status = FRAME_LEN_INVALID;
    int                     ret = -1;    
    
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(FPGA_REMOTE_UPDATE_PORT);
    server.sin_addr.s_addr = inet_addr(FPGA_REMOTE_UPDATE_IP);

    if ((sendLen = fpgaRmtUdtPack(fc, sendPayload, pLen, sendFrame)) < 0)
    {
        trace(DEBUG_ERR, SYSTEM, "%s : fpgaRmtUdtPack error, len = %d", __func__, sendLen);
        return -1;
    }
    
    for (retrytimes = 0; retrytimes < RETRY_TIMES_MAX; retrytimes++)
    {
//        printbuffer ("send a frame : ", sendFrame, sendLen);

        if((ret = sendto(sockfd, sendFrame, sendLen, 0, (struct sockaddr *)&server, sizeof(struct sockaddr_in))) < 0)
        {
            trace(DEBUG_ERR, SYSTEM, "%s : sendto error, ret = %d", __func__, ret);
            trace(DEBUG_ERR, SYSTEM, "%s : errno %d : %s" , __func__, errno, strerror(errno));
            trace(DEBUG_ERR, SYSTEM, "%s : sockfd %d, len %d" , __func__, sockfd, sendLen);
            return -1;
        }

        if ((recvLen = recvfrom(sockfd, recvFrame, sizeof(recvFrame), 
                0, (struct sockaddr * __restrict__)&server, (socklen_t * __restrict__)&fromLen)) < 0)
        {
            trace(DEBUG_ERR, SYSTEM, "%s : recvfrom error(timeout = %dsec), recvLen = %d", __func__, RECV_TIMEOUT_SEC, recvLen);
            break;
        }
//        printbuffer ("recv a frame : ", recvFrame, recvLen);

        if (FRAME_OK == (status = fpgaRmtUdtUnPack(recvFrame, recvLen, &head, data)))
            break;
        else
            trace(DEBUG_ERR, SYSTEM, "%s : fpgaRmtUdtUnPack error, status %d, retrytimes %d", __func__, status, retrytimes);
    }

    if (FRAME_OK != status)
        return -1;

    switch (fc)
    {
        case FC_QUERY_VERSION:
            if (head.fc == FC_QUERY_VERSION_RESP)
                ret = data[0];
            break;
        case FC_UPDATE_START:
            if (head.fc == FC_UPDATE_START_RESP)
                ret = data[0];
            break;
        case FC_UPDATE_END:
            if (head.fc == FC_UPDATE_END_RESP)
                ret = data[0];
            break;
        case FC_VERSION_SW:
            if (head.fc == FC_VERSION_SW_RESP)
                ret = data[0];
            break;
        case FC_TRANS:
            if (head.fc == FC_TRANS_RESP)
                ret = data[0];
            break;
        case FC_ACCESS_REG:
            if (head.fc == FC_ACCESS_REG_RESP)
            {
                if (sendPayload[0] == 1)//write
                    ret = 1;
                else//read
                    ret = (data[3] << 24) + (data[2] << 16) + (data[1] << 8) + data[0];
            }       
            break;        
        default:
            break;
    }
    
    return ret;
}

int fpgaRmtUdt (char *fileName)
{
    int                sockfd;
    FILE               *fp;
    int                ret = -1;
    u8                 payload[PAYLOAD_LENGTH_MAX];
    int                /*ver0, ver1,*/ resp;
    struct stat        s;
    int                pkgNum, pkgIndex = 0, pkgLength;
    struct timeval     tv;

    system("arp -i eth0 -s 192.168.0.2 00:0a:35:01:fe:c0");
    system("ifconfig eth0:1 192.168.0.1 up");
    
    trace (DEBUG_INFO, SYSTEM, "%s : start", __func__);
    
    fpgaCtrl (OPCODE_NET0_FPGA_UPDATE_ON);

//    trace (DEBUG_INFO, SYSTEM, "%s : wait 1 second ", __func__);
//    sleep(1);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
	    trace(DEBUG_ERR, SYSTEM, "%s : open socket err", __func__);
		goto exit;
	}
    tv.tv_sec = RECV_TIMEOUT_SEC;
    tv.tv_usec = 0;
    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)))
    {
        trace(DEBUG_ERR, SYSTEM, "%s : setsockopt set recv out time error", __func__);
        goto exit;
    }

    trace (DEBUG_INFO, SYSTEM, "%s : start update", __func__);
    /* start update */
    payload[0] = 0;
    if ((resp = fpgaRmtUdtRequest(sockfd, FC_UPDATE_START, payload, 1)) != 1)
    {
        trace(DEBUG_ERR, SYSTEM, "%s : start update failed", __func__);
        goto closeSocket;
    }

    trace (DEBUG_INFO, SYSTEM, "%s : start trans upgrade file", __func__);
    /* trans upgrade file */
    if (0 != stat(fileName, &s))
    {
        trace(DEBUG_ERR, SYSTEM, "%s : stat %s failed", __func__, fileName);
        goto updateEnd;
    }
    pkgNum = (s.st_size + PKG_LENGTH_MAX - 1) / PKG_LENGTH_MAX;
    payload[0] = (u8)(pkgNum & 0xff);
    payload[1] = (u8)((pkgNum >> 8) & 0xff);
    payload[2] = (u8)((pkgNum >> 16) & 0xff);
    payload[3] = (u8)((pkgNum >> 24) & 0xff);

    if (NULL == (fp = fopen(fileName, "rb")))
    {
        trace(DEBUG_ERR, SYSTEM, "%s : fopen %s failed", __func__, fileName);
        goto updateEnd;
    }

    do
    {
        memset(&payload[4], 0, PKG_LENGTH_MAX + 8);
        if (0 < (pkgLength = fread(&payload[12], 1, PKG_LENGTH_MAX, fp)))
        {
            pkgIndex++;
            payload[4] = (u8)(pkgIndex & 0xff);
            payload[5] = (u8)((pkgIndex >> 8) & 0xff);
            payload[6] = (u8)((pkgIndex >> 16) & 0xff);
            payload[7] = (u8)((pkgIndex >> 24) & 0xff);
            payload[8] = (u8)(pkgLength & 0xff);
            payload[9] = (u8)((pkgLength >> 8) & 0xff);
            payload[10] = (u8)((pkgLength >> 16) & 0xff);
            payload[11] = (u8)((pkgLength >> 24) & 0xff);
            
            if ((resp = fpgaRmtUdtRequest(sockfd, FC_TRANS, payload, (pkgLength + 12))) != 1)
            {
                trace(DEBUG_ERR, SYSTEM, "%s : trans pkgIndex %d pkgLength %d failed", 
                    __func__, pkgIndex, pkgLength);
                goto closeFile;
            }
        }
    }while(pkgLength > 0);
    trace (DEBUG_INFO, SYSTEM, "%s : trans upgrade file over", __func__);

closeFile:
    fclose(fp);

updateEnd:
    trace (DEBUG_INFO, SYSTEM, "%s : end update", __func__);
    /* end update */
    if ((ret = fpgaRmtUdtRequest(sockfd, FC_UPDATE_END, NULL, 0)) == -1)
    {
        trace(DEBUG_ERR, SYSTEM, "%s : end update failed", __func__);
        goto closeSocket;
    }

//    /* version switch */
//    trace (DEBUG_INFO, SYSTEM, "%s : version switch", __func__);
//    payload[0] = 0;
//    if ((resp = fpgaRmtUdtRequest(sockfd, FC_VERSION_SW, payload, 1)) != 1)
//    {
//        trace(DEBUG_ERR, SYSTEM, "%s : start update failed.", __func__);
//        goto closeSocket;
//    }
//    /* wait a moment */
//    trace(DEBUG_INFO, SYSTEM, "%s : Wait 1 seconds.", __func__);
//    sleep(1);

    ret = 0;

closeSocket:
    close(sockfd);
exit:
    trace (DEBUG_INFO, SYSTEM, "%s : end", __func__);
    fpgaCtrl (OPCODE_NET0_FPGA_UPDATE_OFF);
    system("ifconfig eth0:1 down");

    return ret;
}

int fpgaGetVersion (void)
{
    int                sockfd;
    int                ret = -1;
    struct timeval     tv;

    system("arp -i eth0 -s 192.168.0.2 00:0a:35:01:fe:c0");
    system("ifconfig eth0:1 192.168.0.1 up");
    
    fpgaCtrl (OPCODE_NET0_FPGA_UPDATE_ON);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
	    trace(DEBUG_ERR, SYSTEM, "%s : open socket err", __func__);
        goto exit;
	}
    tv.tv_sec = RECV_TIMEOUT_SEC;
    tv.tv_usec = 0;
    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)))
    {
        trace(DEBUG_ERR, SYSTEM, "%s : setsockopt set recv out time error", __func__);
        goto exit;
    }

    /* query logic version */
    if ((ret = fpgaRmtUdtRequest(sockfd, FC_QUERY_VERSION, NULL, 0)) == -1)
    {
        trace(DEBUG_ERR, SYSTEM, "%s : query version failed", __func__);
        goto closeSocket;
    }

closeSocket:
    close(sockfd);
exit:
    fpgaCtrl (OPCODE_NET0_FPGA_UPDATE_OFF);
    system("ifconfig eth0:1 down");
    return ret;
}


