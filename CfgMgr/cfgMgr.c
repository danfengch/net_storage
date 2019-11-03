/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     cfgMgr.c
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/09/25
 * @defgroup cfgMgr
 * @ingroup  cfgMgr  
 * @brief    Configure Management Unit Function implement
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/09/25    Chenxu        None              File Create
 ****************************************************************************** 
 */
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>  
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <pthread.h>

extern pthread_t webThreadId;

extern int webInit (void);

int main(int argc, char * argv[])
{   
    webInit();

    pthread_join(webThreadId, NULL);
    
    return 0;
}


