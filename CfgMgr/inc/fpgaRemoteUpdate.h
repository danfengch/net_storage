/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     cfgMgr.c
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/09/25
 * @defgroup cfgMgr
 * @ingroup  fpgaRemoteUpdate  
 * @brief    fpga remote update protocol v1.01
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/11/22    Chenxu        None              File Create
 ****************************************************************************** 
 */
#ifndef __FPGA_REMOTE_UPDATE_H__
#define __FPGA_REMOTE_UPDATE_H__

#include <sys/ioctl.h>

#define FC_UPDATE_START         0xAA
#define FC_UPDATE_START_RESP    0xAB
#define FC_TRANS                0xAC
#define FC_TRANS_RESP           0xAD
#define FC_UPDATE_END           0xAE
#define FC_UPDATE_END_RESP      0xAF
#define FC_QUERY_VERSION        0xB0
#define FC_QUERY_VERSION_RESP   0xB1
#define FC_VERSION_SW           0xB2
#define FC_VERSION_SW_RESP      0xB3
#define FC_ACCESS_REG           0xC0
#define FC_ACCESS_REG_RESP      0xC1

#define SD2_CONST               0x68
#define DA_CONST                0xFF
#define SA_CONST                0x02
#define ED_CONST                0x16

/*固件更新开始*/
#define LOGIC_AREA_0            0x00
#define LOGIC_AREA_1            0x01
#define LOGIC_AREA_2            0x02

/*逻辑版本查询（当前FPGA加载的逻辑版本）*/
#define LOGIC_VERSION_0         0x00
#define LOGIC_VERSION_1         0x01
#define LOGIC_VERSION_2         0x02

#define PKG_LENGTH_MAX          1024

#define PAYLOAD_LENGTH_MIN      0
#define PAYLOAD_LENGTH_MAX      (PKG_LENGTH_MAX + 12)           //FC_TRANS
#define RETRY_TIMES_MAX         3
#define LE_MIN                  (3 + PAYLOAD_LENGTH_MIN)
#define LE_MAX                  (LE_MIN + PAYLOAD_LENGTH_MAX)
#define FRAME_LEN_MIN           (LE_MIN + 8)
#define FRAME_LEN_MAX           (LE_MAX + 8) 

typedef unsigned char           u8;
typedef unsigned short          u16;



/* header define */
typedef struct __attribute__((__aligned__(1)))
{
    u8 sd2_0;
    u8 le_byte0;
    u8 le_byte1;
    u8 ler_byte0;
    u8 ler_byte1;
    u8 sd2_1;
    u8 da;
    u8 sa;
    u8 fc;
}fgpaRemoteUpdateHeader;

typedef enum
{
    FRAME_OK = 0,
    FRAME_LEN_INVALID = -1,
    FRAME_SD2_0_INVALID = -2,
    FRAME_LE_INVALID = -3,
    FRAME_SD2_1_INVALID = -4,
    FRAME_FC_INVALID = -5,
    FRAME_FCS_INVALID = -6,
    FRAME_ED_INVALID = -7
}frameStatus;

extern int fpgaRmtUdt (char *fileName);

#endif /*__FPGA_REMOTE_UPDATE_H__*/
