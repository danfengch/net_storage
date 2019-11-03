/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     trace.h
  * @author   Chenxu
  * @version  V1.0.0
  * @date     2019/09/30
  * @defgroup cfgMgr
  * @ingroup  log  
  * @brief    log function declear
  * @par History
  * Date          Owner         BugID/CRID        Contents
  * 2019/09/30    Chenxu        None              File Create
  ****************************************************************************** 
  */
#ifndef __TRACE_H__
#define __TRACE_H__

#include <time.h>
#include <config.h>


#define DEBUG_ERR          1
#define DEBUG_WARN         2
#define DEBUG_INFO         3

typedef enum
{
    USER,
    SYSTEM,
    LOGTYPE_ALL,
}logType;

typedef enum
{
    LOGSIGNIFICANCE_GENERAL,
    LOGSIGNIFICANCE_KEY,
    LOGSIGNIFICANCE_ALL
}logSignificance;

typedef struct
{
    time_t occurTime;
    logType typ;
    logSignificance sgnfcc;
    char content[LOG_BUF_LEN_MAX + 1];
}logElement;


void logInit(void);

int logRequest(time_t s, time_t e, logType typ, 
    logSignificance sgnfcc, int start, logElement *pElements, int elementMax);

int logRequestExport(time_t s, time_t e, logType typ, logSignificance sgnfcc);

void trace (int logLevel, int typ, const char *fmt, ...);

const char *getLastCfgMgrErr(void);

#endif
