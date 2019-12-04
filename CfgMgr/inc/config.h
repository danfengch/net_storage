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

//#define DEBUG_CFGMGR

#define SYSTEM_V_MQUEUE
//#define MAC_CONFIG
#define NET_CAPTURE
#define KEEP_TIME
#define FEED_WATCHDOG
#define CAPTURE_STATISTICS

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

#define NET_FILES_PATH                   "/usr/httproot/NetFiles/"

#define VERSION_STRING_LEN_MAX           10

#ifndef DEBUG_CFGMGR
#define LOG_DATA_BASE_FILE_NAME          "/usr/httproot/flashdev/cfgmgrLog.db"
#define LOG_EXPORT_FILE_NAME             "/usr/httproot/tmp/logSearchResult.txt"
#define CONFIG_FILE_NAME                 "/usr/httproot/cfg/config.xml"
#else
#define LOG_DATA_BASE_FILE_NAME          "/home/chenxu/LOONGSON-2k1000/src/web/flashdev/cfgmgrLog.db"
#define LOG_EXPORT_FILE_NAME             "/home/chenxu/LOONGSON-2k1000/src/web/tmp/logSearchResult.txt"
#define CONFIG_FILE_NAME                 "/home/chenxu/LOONGSON-2k1000/src/web/cfg/config.xml"
#endif
#define CFGMGR_UPDATE_FILE_NAME          "/usr/httproot/updatefiles/CfgMgr_new"
#define CFGMGR_BAK_FILE_NAME             "/usr/httproot/updatefiles/CfgMgr_old"
#define CFGMGR_RUN_FILE_NAME             "/usr/sbin/CfgMgr"

#define FPGA_UPDATE_FILE_NAME            "/usr/httproot/updatefiles/FpgaConfigFile_new"
#define FPGA_BAK_FILE_NAME               "/usr/httproot/updatefiles/FpgaConfigFile_old"

#define WEB_UPDATE_FILE_NAME             "/usr/httproot/updatefiles/Web_new"
#define WEB_BAK_FILE_NAME                "/usr/httproot/updatefiles/Web_old"

#define STORAGE_PATH                     "/usr/httproot/NetFiles/"


/* eeprom */
#define I2C_ADDR_OF_EEP                  0x50
#define EEP_ADDR_TIMESTAMP               128
#define TIMESTAMP_LEN                    4

/* fpga remote update */
#define FPGA_REMOTE_UPDATE_PORT          8080
#define FPGA_REMOTE_UPDATE_IP            "192.168.0.2"



#endif
