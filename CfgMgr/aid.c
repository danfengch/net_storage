/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     cfgMgr.c
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/09/25
 * @defgroup cfgMgr
 * @ingroup  aid  
 * @brief    Configure Management Unit Function implement
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/11/09    Chenxu        None              File Create
 ****************************************************************************** 
 */
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>  
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/times.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <trace.h>
#include <assert.h>
#include <share.h>

#define I2C_DEV                 "/dev/i2c-0"
#define I2C_DEFAULT_TIMEOUT     1
#define I2C_DEFAULT_RETRY       3



pthread_t aidThreadId;
static time_t lastTime = 0;
unsigned int sysRuntime = 0;
int rebootFlag = 0;

#ifdef FEED_WATCHDOG
static void watchdogInit(void)
{
    system("echo 20 > /sys/class/gpio/export");
    system("echo \"out\" > /sys/class/gpio/gpio20/direction");
}

static void feedWatchDog(void)
{
    static int pinValue = 0;
    char cmd[50] = {0};

    if (!rebootFlag)
    {
        snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio20/value", !pinValue);
        system(cmd);
        pinValue = !pinValue;
    }
}
#endif

#ifdef KEEP_TIME
int i2c_set(int fd, unsigned int timeout, unsigned int retry)
{
    if (fd == 0 )
        return -1;

    if (ioctl(fd, I2C_TIMEOUT, timeout ? timeout : I2C_DEFAULT_TIMEOUT) < 0)
        return -1;
    if (ioctl(fd, I2C_RETRIES, retry ? retry : I2C_DEFAULT_RETRY) < 0)
        return -1;

    return 0;
}

int i2c_nbytes_write(int fd, unsigned char addr, unsigned char reg, unsigned char *val, int len)
{
    int ret = 0;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages;
    int i;

    packets.nmsgs = 1;
    packets.msgs = &messages;

    //发送要读取的寄存器地址
    messages.addr = addr;
    messages.flags = 0;         //write
    messages.len = len + 1;     //数据长度
    //发送数据
    messages.buf = (unsigned char *)malloc(len+1);
    if (NULL == messages.buf)
    {
        ret = -1;
        goto err;
    }

    messages.buf[0] = reg;
    for (i = 0; i < len; i++)
    {
        messages.buf[1+i] = val[i];
    }

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&packets);//读出来
    if (ret < 0){
        printf("write error!\n");
        return -1;
    }

err:
    free(messages.buf);

    return ret;
}

int i2c_nbytes_read(int fd, unsigned char addr, unsigned char reg, unsigned char *val, int len)
{
    int ret = 0;
    unsigned char outbuf;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    /* 数据帧类型有2种
     * 写要发送起始信号，进行写寄存器操作，再发送起始信号,进行读操作,
     * 有2个起始信号，因此需要分开来操作。
     */
    packets.nmsgs = 2;           
    //发送要读取的寄存器地址
    messages[0].addr = addr;
    messages[0].flags = 0;              //write
    messages[0].len = 1;                //数据长度
    messages[0].buf = &outbuf;          //发送寄存器地址
    outbuf = reg;
    //读取数据
    messages[1].len = len;                           //读取数据长度
    messages[1].addr = addr;                         //设备地址
    messages[1].flags = I2C_M_RD;                    //read
    messages[1].buf = val;

    packets.msgs = messages;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&packets); //发送i2c,进行读取操作 
    if (ret < 0)
        ret = -1;

    return ret;
}

static time_t sysTimeReadFormBoard(void)
{
    time_t ti = 0;
    int    fd;

    if ((fd =  open (I2C_DEV, O_RDONLY)) > 0)
    {
        //set timeout & reties times
        if(i2c_set(fd, 3, 2) == -1 )
        {
            trace(DEBUG_ERR, SYSTEM, "SET %s PARAM ERR.", I2C_DEV);
            goto sysTimeReadFormBoardExit;
        }

        if(-1 == i2c_nbytes_read(fd, I2C_ADDR_OF_EEP, EEP_ADDR_TIMESTAMP, 
            (unsigned char *)&ti, sizeof(time_t)))
        {
            trace(DEBUG_ERR, SYSTEM, "READ TIME FROM %s ERR.", I2C_DEV);
            goto sysTimeReadFormBoardExit;
        }

        close(fd);
    }
    else
    {
        trace(DEBUG_ERR, SYSTEM, "OPEN %s ERR.", I2C_DEV);
    }

    
sysTimeReadFormBoardExit:
    return ti;
}

static int sysTimeWriteToBoard(time_t t)
{
    int    fd;
    
    if ((fd =  open (I2C_DEV, O_WRONLY)) > 0)
    {
        //set timeout & reties times
        if(i2c_set(fd, 3, 2) == -1 )
        {
            trace(DEBUG_ERR, SYSTEM, "SET %s PARAM ERR.", I2C_DEV);
            goto sysTimeWriteToBoardExit;
        }

        if(-1 == i2c_nbytes_write(fd, I2C_ADDR_OF_EEP, EEP_ADDR_TIMESTAMP, 
            (unsigned char *)&t, sizeof(time_t)))
        {
            trace(DEBUG_ERR, SYSTEM, "READ TIME FROM %s ERR.", I2C_DEV);
            goto sysTimeWriteToBoardExit;
        }

        close(fd);
    }
    else
    {
        trace(DEBUG_ERR, SYSTEM, "OPEN %s ERR.", I2C_DEV);
    }


sysTimeWriteToBoardExit:
    return 0;
}

#define KEEPTIME_INTERVAL_SEC        60
void keepTime(void)
{	
	time_t curtime;

    curtime = time(NULL);
    if(abs(curtime - lastTime) >= KEEPTIME_INTERVAL_SEC)
	{        	
        lastTime = curtime;

        sysTimeWriteToBoard(lastTime);
//        printf("%s : sysTimeWriteToBoard %s\n", __FUNCTION__, time2format2(lastTime));
	}
}
#endif

void aidProcess(void *arg)
{   
    while(1)
    {
        //sleep 500ms
        usleep(500000);

        /* feed watchdog */
#ifdef FEED_WATCHDOG
        feedWatchDog();
#endif
        /* keep current time to Non-volatile Memory Device */
#ifdef KEEP_TIME
        keepTime();
#endif
    }
}

void aidInit(void)
{
    int ret;
    pthread_attr_t attr;

#ifdef KEEP_TIME
    lastTime = sysTimeReadFormBoard();

    printf("lastTime : %s\n", time2format2(lastTime));

    stime(&lastTime);
#endif
    
#ifdef FEED_WATCHDOG
    watchdogInit();
#endif

    ret = pthread_attr_init(&attr);
    assert(ret == 0);
    ret = pthread_attr_setstacksize(&attr, WEB_THREAD_STACK_SIZE);
    assert(ret == 0);
    
    ret = pthread_create(&aidThreadId, &attr, (void *) aidProcess, NULL);
    if(ret != 0)
        trace(DEBUG_ERR, SYSTEM, "Create pthread error[%d]!", ret);
}

