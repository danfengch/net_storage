#include <fpgaCtrl.h>
#include <dirent.h>
#include <config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parameters.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/prctl.h>
#include <cfgMgr.h>
#include <linux/if.h>  
#include <linux/ethtool.h> 
#include <linux/sockios.h>
#include <net/route.h>
#include <net/if_arp.h>
#include <trace.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cfgMgrMessage.h>
#include <assert.h>
#include <mxml.h>
#include <version.h>
#include <share.h>
#include <aid.h>
#include <sys/types.h>
#include <fpgaRemoteUpdate.h>


/*
1.功能编码
    Gpio18	Gpio0	Gpio1	Gpio2	说明
    0	0	0	0	通道0 不更新（给陈伦）
        0	0	1	通道0 10M
        0	1	0	通道0 更新（给陈伦）
        0	1	1	通道0 100M
        1	0	0	无
        1	0	1	通道0 1000M
        1	1	1	打开过滤
        1	1	0	关闭过滤
    1	0	0	0	通道1 不更新（给陈伦）
        0	0	1	通道1  10M
        0	1	0	通道1 更新（给陈伦）
        0	1	1	通道1  100M
        1	0	0	无
        1	0	1	通道1  1000M
        1	1	1	打开过滤
        1	1	0	关闭过滤
2.使能管脚
    gpio19 作为使能信号使用，高电平有效
*/
static const int opCode[][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {0, 1, 1, 1},
    {0, 1, 1, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 1},
    {1, 0, 1, 0},
    {1, 0, 1, 1},
    {1, 1, 0, 0},
    {1, 1, 0, 1},
    {1, 1, 1, 1},
    {1, 1, 1, 0},
};

void fpgaCtrlInit(void)
{
    DIR *dir;

    if (NULL == (dir = opendir("/sys/class/gpio/gpio19")))
    {
        system("echo 19 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio19/direction");    
    }
    else
    {
        closedir(dir);
    }
    system("echo 0 > /sys/class/gpio/gpio19/value");

    if (NULL == (dir = opendir("/sys/class/gpio/gpio0")))
    {
        system("echo 0 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio0/direction");
    }
    else
    {
        closedir(dir);
    }

    if (NULL == (dir = opendir("/sys/class/gpio/gpio1")))
    {
        system("echo 1 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio1/direction");
    }
    else
    {
        closedir(dir);
    }

    if (NULL == (dir = opendir("/sys/class/gpio/gpio2")))
    {
        system("echo 2 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio2/direction");
    }
    else
    {
        closedir(dir);
    }

    if (NULL == (dir = opendir("/sys/class/gpio/gpio18")))
    {
        system("echo 18 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio18/direction");    
    }
    else
    {
        closedir(dir);
    }
}

void fpgaCtrl (fpgaOperateCode code)
{
    char cmd[50];
    
    system("echo 0 > /sys/class/gpio/gpio19/value");
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio18/value", opCode[code][0]);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio0/value", opCode[code][1]);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio1/value", opCode[code][2]);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio2/value", opCode[code][3]);
    system(cmd);
    system("echo 1 > /sys/class/gpio/gpio19/value");
    usleep(1000);
    system("echo 0 > /sys/class/gpio/gpio19/value");
}

