/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     trace.c
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
#include <trace.h>
#include <string.h>
#include <sys/prctl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <config.h>
#include <pthread.h>
#include <assert.h>
#include <share.h>


#define LOG_LOCK 	    		pthread_mutex_lock(&logMutex)
#define LOG_UNLOCK  			pthread_mutex_unlock(&logMutex)

#define DEBUG_LEVEL             DEBUG_INFO


static pthread_mutex_t logMutex;
static char lastCfgMgrErr[LOG_BUF_LEN_MAX + 1] = {0};


static sqlite3 *logOpen(char *fileName)
{
	sqlite3 *db;
	int result;
	char *errmsg = NULL;
	char *sql = "create table if not exists logtable( \
		OccurTime, \
		LogType,\
		Significance,\
		Content\
		)";

    result = sqlite3_open(fileName, &db);  
	if(result != SQLITE_OK)
	{
		printf("Can't open datebase(sqlite_errmsg : %s)\n", sqlite3_errmsg(db));
		return NULL;
	}
	result = sqlite3_exec(db, sql,0,0, &errmsg);

	if(result != SQLITE_OK)
		printf("warning:Create table fail! May table \
		    logtable already result[%d] errmsg[%s].\n", result,  errmsg);

	return db;
}

static void logClose(sqlite3 * db)
{
    sqlite3_close(db);
}

static void logWrite(logType typ, logSignificance sgnfcc, char *content)
{

#define MAX_MESSAGE_LEN		20
    sqlite3 *db;
	int result;
	char *errmsg = NULL;
	char sql[200];
	time_t tinow = time(NULL);

    LOG_LOCK;

	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "insert into logtable values('%d','%d','%d','%s');", 
		(int)tinow, (int)typ, (int)sgnfcc, content);

    db = logOpen(LOG_DATA_BASE_FILE_NAME);
	assert(db);
	result = sqlite3_exec(db,sql,0,0,&errmsg);
	if(result != SQLITE_OK) 
	{
		printf("Can't insert into datebase result[%d] errmsg[%s], sql[%s]\n", result,errmsg,sql);
		goto Write_Err;
	}
//    result = sqlite3_complete(sql);
//    if(result != SQLITE_OK) 
//	{
//		printf("sqlite3_complete err : result[%d] errmsg[%s], sql[%s]\n", result,errmsg,sql);
//		goto Write_Err;
//	}

Write_Err:
    logClose(db);
    
	LOG_UNLOCK;
}

static char **logSelect(time_t s, time_t e, logType typ, logSignificance sgnfcc, int *nrow, int *ncol)
{
    sqlite3 *db;
    char sql[200];
    int result;
    char *errmsg = NULL;
    char **table = NULL;

    *nrow = 0;
    *ncol = 0;
    
    LOG_LOCK;
    
    if(NULL == (db = logOpen(LOG_DATA_BASE_FILE_NAME)))
    {
        printf("logOpen %s failed\n", LOG_DATA_BASE_FILE_NAME);
        goto logSelectExit;
    }
    
    if ((typ != LOGTYPE_ALL) && (sgnfcc != LOGSIGNIFICANCE_ALL))
        sprintf(sql, "select * from logtable where OccurTime between '%d' and '%d'"
            " and LogType='%d' and Significance='%d'", (int)s, (int)e, (int)typ, sgnfcc);
    else if((typ == LOGTYPE_ALL) && (sgnfcc != LOGSIGNIFICANCE_ALL))
        sprintf(sql, "select * from logtable where OccurTime between '%d' and '%d'"
            " and Significance='%d'", (int)s, (int)e, (int)sgnfcc);
    else if((typ != LOGTYPE_ALL) && (sgnfcc == LOGSIGNIFICANCE_ALL))
        sprintf(sql, "select * from logtable where OccurTime between '%d' and '%d'"
            " and LogType='%d'", (int)s, (int)e, (int)typ);
    else
    {
        sprintf(sql, "select * from logtable where OccurTime between '%d' and '%d'", (int)s, (int)e);           
    }
//    printf("sql %s\n", sql);
    
    result = sqlite3_exec(db,sql,0,0,&errmsg);
    if(result != SQLITE_OK) 
    {
        printf("%s failed, result[%d] errmsg[%s]\n", sql, result, errmsg);
        goto logSelectExit;
    }
    
    sqlite3_get_table(db,sql,&table,nrow,ncol,&errmsg);

logSelectExit:
    if (db)
    {
        logClose(db);
    }
    
    LOG_UNLOCK;
    
    if(nrow == 0)
    {
        if (table)
        {
            sqlite3_free_table(table);
        }
        return NULL;
    }
    else
        return table;
}


int logRequest(time_t s, time_t e, logType typ, 
    logSignificance sgnfcc, int start, logElement *pElements, int elementMax)
{
	int nrow = 0, ncol = 0, i, element;
	char **table = NULL;

    if (NULL == (table = logSelect(s, e, typ, sgnfcc, &nrow, &ncol)))
    {
        nrow = 0;
        goto logRequestExit;
    }
    
//    printf("nrow = %d\n", nrow);
	for(i = 1 + start, element = 0; (i <= nrow) && (element < elementMax); i++, element++) 
	{
//	    printf("%-10s    %-3s    %-3s    %-40s\n", 
//            table[i*ncol+0], table[i*ncol+1], table[i*ncol+2], table[i*ncol+3]);
        pElements[element].occurTime = (time_t)atoi(table[i*ncol+0]);
        pElements[element].typ = (logType)atoi(table[i*ncol+1]);
        pElements[element].sgnfcc = (logSignificance)atoi(table[i*ncol+2]);
        strncpy(pElements[element].content, table[i*ncol+3], LOG_BUF_LEN_MAX);
	}

logRequestExit:
    if (table)
    {
    	sqlite3_free_table(table);
    }
    
	return (nrow);
}

int logRequestExport(time_t s, time_t e, logType typ, logSignificance sgnfcc)
{
	int nrow = 0, ncol = 0, i = 0;
	char **table = NULL;
    FILE *fp;
    char timeFmt[50];
    logElement log;

    if (NULL == (fp = fopen(LOG_EXPORT_FILE_NAME, "w")))
        goto logRequestExportExit;

    if (NULL == (table = logSelect(s, e, typ, sgnfcc, &nrow, &ncol)))
        goto logRequestExportExit;

    fprintf(fp, "%-25s%-10s%-15s%-50s\r\n", table[0], table[1], table[2], table[3]);
    fprintf(fp, "------------------------------------------------------------------------------------\r\n");
	for(i = 1; i < nrow+1; i++) 
	{
        log.occurTime = (time_t)atoi(table[i*ncol+0]);
        time2format(log.occurTime, timeFmt);
        log.typ = (logType)atoi(table[i*ncol+1]);
        log.sgnfcc = (logSignificance)atoi(table[i*ncol+2]);

        fprintf(fp, "%-25s%-10s%-15s%-50s\r\n", timeFmt, 
            (log.typ == USER) ? "user" : "system",
            (log.sgnfcc == LOGSIGNIFICANCE_GENERAL) ? "genarl" : "key",
            table[i*ncol+3]);
	}

    if(nrow > 0)
        nrow--;

logRequestExportExit:
    if (table)
    {
    	sqlite3_free_table(table);
    }
    if (fp)
    {
        fclose(fp);
    }
    
	return (nrow);
}


void logInit( void )
{
    if(0 != pthread_mutex_init(&logMutex, NULL))
	{
		printf("create logMutex failed\n");
	}
}

void trace(int logLevel, int typ, const char *fmt, ...)
{
    va_list ap;
    logSignificance sgnfcc;
    time_t timeNow = time(NULL);

    va_start(ap, fmt);
    vsnprintf(lastCfgMgrErr, LOG_BUF_LEN_MAX, fmt, ap);
    va_end(ap);

    printf("[ %s ] ", time2format2(timeNow));
    printf(lastCfgMgrErr);
    printf("\n");

    if (logLevel <= DEBUG_LEVEL)
    {
        if (logLevel <= DEBUG_ERR)
            sgnfcc = LOGSIGNIFICANCE_KEY;
        else
            sgnfcc = LOGSIGNIFICANCE_GENERAL;
        
        logWrite(typ, sgnfcc, lastCfgMgrErr);
    }
}

const char *getLastCfgMgrErr(void)
{
    return (const char *)lastCfgMgrErr;
}



