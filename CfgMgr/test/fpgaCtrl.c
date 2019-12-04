#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*
1.功能编码
    Gpio18	Gpio0	Gpio1	Gpio2	说明
    0	0	0	0	通道0 更新（给陈伦）
        0	0	1	通道0 10M
        0	1	0	通道0 不更新（给陈伦）
        0	1	1	通道0 100M
        1	0	0	无
        1	0	1	通道0 1000M
        1	1	1	打开过滤
        1	1	0	关闭过滤
    1	0	0	0	通道1 更新（给陈伦）
        0	0	1	通道1  10M
        0	1	0	通道1 不更新（给陈伦）
        0	1	1	通道1  100M
        1	0	0	无
        1	0	1	通道1  1000M
        1	1	1	打开过滤
        1	1	0	关闭过滤
2.使能管脚
    gpio19 作为使能信号使用，高电平有效

*/
static void fgpaCtrlInit(void)
{
    system("echo 19 > /sys/class/gpio/export");
    system("echo \"out\" > /sys/class/gpio/gpio19/direction");
    system("echo 0 > /sys/class/gpio/gpio19/value");
    
    
    system("echo 0 > /sys/class/gpio/export");
    system("echo \"out\" > /sys/class/gpio/gpio0/direction");
    
    system("echo 1 > /sys/class/gpio/export");
    system("echo \"out\" > /sys/class/gpio/gpio1/direction");
    
    system("echo 2 > /sys/class/gpio/export");
    system("echo \"out\" > /sys/class/gpio/gpio2/direction");
    
    system("echo 18 > /sys/class/gpio/export");
    system("echo \"out\" > /sys/class/gpio/gpio18/direction");    
}

static void fpgaCtrlUnInit(void)
{
    system("echo 0 > /sys/class/gpio/unexport");
    system("echo 1 > /sys/class/gpio/unexport");
    system("echo 2 > /sys/class/gpio/unexport");
    system("echo 18 > /sys/class/gpio/unexport");
    system("echo 19 > /sys/class/gpio/unexport");
}

void printUsage(void)
{
    printf("Useage : fpgaCtrl gpio18 gpio0 gpio1 gpio2 \n");
}

int main (int argc, char **argv)
{
    int ret = -1;
    int gpio18, gpio0, gpio1, gpio2;
    char cmd[50];
    
    if (argc != 5)
    {
        printUsage();
        goto exit;
    }
    
    gpio18 = atoi(argv[1]);
    gpio0  = atoi(argv[2]);
    gpio1  = atoi(argv[3]);
    gpio2  = atoi(argv[4]);
    
    printf("%d %d %d %d\n", gpio18, gpio0, gpio1, gpio2);
    
    fgpaCtrlInit();
    
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio18/value", gpio18);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio0/value", gpio0);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio1/value", gpio1);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio2/value", gpio2);
    system(cmd);
    
    system("echo 1 > /sys/class/gpio/gpio19/value");
    usleep(10000);
    system("echo 0 > /sys/class/gpio/gpio19/value");
    
exit:
    return ret;
}