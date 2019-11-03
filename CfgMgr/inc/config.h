/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     config.h
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/09/25
 * @defgroup cfgMgr
 * @ingroup  cfgMgr  
 * @brief    config
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/09/25    Chenxu        None              File Create
 ****************************************************************************** 
 */
 #ifndef __CONFIG_H__
 #define __CONFIG_H__

#define HOST_CX                          1
#define HOST_ZHANG                       2
#define HOST_LOONGSON                    3

#define HOST                             HOST_LOONGSON
//#define HOST                             HOST_CX

#define ADMIN_USER_NUM                   1
#define NORMAL_USER_NUM                  1
#define USER_NUM                         (ADMIN_USER_NUM + NORMAL_USER_NUM)
#define WEB_THREAD_STACK_SIZE            (2 * 1024 *1024)

#if (HOST == HOST_CX)
#define NET1_NAME                        "enp0s31f6"
#define NET2_NAME                        "null"
#elif (HOST == HOST_ZHANG)
#define NET1_NAME                        "wlo1"
#define NET2_NAME                        "null"
#elif (HOST == HOST_LOONGSON)
#define NET1_NAME                        "eth0"
#define NET2_NAME                        "eth1"
#else
#error "host config error !!!"
#endif

#define FILE_NAME_LEN_MAX                60

#define LOG_BUF_LEN_MAX                  FILE_NAME_LEN_MAX

#define PAGE_RECORDS_MAX                 50

#define NET_FILES_PATH                   "../NetFiles/"

#define VERSION_STRING_LEN_MAX           10

#define LOG_DATA_BASE_FILE_NAME          "../flashdev/cfgmgrLog.db"
#define LOG_EXPORT_FILE_NAME             "../tmp/logSearchResult.txt"


#endif
