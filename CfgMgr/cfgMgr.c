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
#include <cfgMgrMessage.h>
#include <version.h>

extern pthread_t webThreadId;
extern pthread_t aidThreadId;

extern int webInit (void);
extern void aidInit(void);

int main(int argc, char * argv[])
{   

    printf("   VERSION : %d.%d\n", CFGMGR_MAJOR, CFGMGR_MINOR);
    printf("BUILD TIME : %s %s\n", __DATE__, __TIME__);

    aidInit();
    
    webInit();

    pthread_join(aidThreadId, NULL);
    pthread_join(webThreadId, NULL);
    
    return 0;
}


