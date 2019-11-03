/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     message.h
  * @author   Chenxu
  * @version  V1.0.0
  * @date     2019/09/25
  * @defgroup cfgMgr
  * @ingroup  cfgMgr
  * @brief    message declare
  * @par History
  * Date          Owner         BugID/CRID        Contents
  * 2019/09/25    Chenxu        None              File Create
  ****************************************************************************** 
  */
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <config.h>
#include <mqueue.h>
#include <parameters.h>
#include <trace.h>


#define MSG_DATA_LEN_MAX        4096

#define CGI_CFGMGR_MSG_NAME     "/cgiCfgMgrMessage"
#define CFGMGR_DUMPCAP_MSG0_NAME "/cfgmgrDumpcapMessage0"
#define CFGMGR_DUMPCAP_MSG1_NAME "/cfgmgrDumpcapMessage1"


//typedef enum
//{
//}cfgmgrDumpcapMsgType;


typedef enum
{
    MSGTYPE_COMFIRM = 0,
    MSGTYPE_LOGIN_REQUEST,
    MSGTYPE_LAN1TEST,
    MSGTYPE_LAN2TEST,
    MSGTYPE_NETCONFIGSAVE,
    MSGTYPE_NETCAPTURE_REQUEST,
    MSGTYPE_NETFILTER_REQUEST,
    MSGTYPE_FILELOOKUP_REQUEST,
    MSGTYPE_FILEUPLOAD_REQUEST,
    MSGTYPE_DISKINFO_REQUEST,
    MSGTYPE_NORMALUSERMGR_REQUEST,
    MSGTYPE_SUPERUSERMGR_REQUEST,
    MSGTYPE_SYSTEMINFO_REQUEST,
    MSGTYPE_LAN1STATISTICSCLEAR_REQUEST,
    MSGTYPE_LAN2STATISTICSCLEAR_REQUEST,
    MSGTYPE_SYSTIMESET_REQUEST,
    MSGTYPE_SYSTIMEGET_REQUEST,
    /** 高级管理 */
    MSGTYPE_ADMINPASSWDCOMFIRM_REQUEST,
    MSGTYPE_GETVERSION_REQUEST,
    MSGTYPE_UPDATELOGICFILE_REQUEST,
    MSGTYPE_UPDATECFGMGRFILE_REQUEST,
    MSGTYPE_UPDATEWEB_REQUEST,
    MSGTYPE_FACTORYRESET_REQUEST,
    MSGTYPE_REBOOT_REQUEST,
    MSGTYPE_LOGLOOKUP_REQUEST,
    MSGTYPE_LOGEXPORT_REQUEST,
    MSGTYPE_LOGCLEARALL_REQUEST,
    

    MSGTYPE_FILELOOKUP_RESPONSE,
    MSGTYPE_FILEUPLOAD_RESPONSE,
    MSGTYPE_DISKINFO_RESPONSE,
    MSGTYPE_SYSTEMINFO_RESPONSE,
    MSGTYPE_SYSTIMEGET_RESPONSE,
    MSGTYPE_GETVERSION_RESPONSE,
    MSGTYPE_LOGLOOKUP_RESPONSE,
    MSGTYPE_LOGEXPORT_RESPONSE,
}msgType;

typedef enum
{
    CFGMGR_OK = 0,
    CFGMGR_ERR,
    CFGMGR_NET_NUMBER_INVALID,
    CFGMGR_IP_INVALID,
    CFGMGR_NOT_SUPPORT,
    CFGMGR_NETWORK_UNREACHABLE,
    CFGMGR_PASSWD_INVALID,
    CFGMGR_USER_NOT_EXIST,
}cfgMgrStatus;

typedef struct
{
    char userName[USR_KEY_LNE_MAX + 1];
    char passwd[USR_KEY_LNE_MAX + 1];
}loginRequest;

typedef struct
{
    netParam net;
    in_addr_t destIp;
}lanTestRequest;

typedef struct
{
    int netNumber;
    time_t startTime;
    time_t endTime;
    int start;          /** Where do we start returning data */
    int length;           /** how many records du we return */
    int draw;           /** only web use */
}fileLookUpRequest;

typedef struct
{
    char fileName[FILE_NAME_LEN_MAX + 1];
    time_t modifyTime;
    int sizeMB;
}fileElement;

typedef struct
{
    int recordsTotal;
    int length;
    int draw;
    fileElement elements[PAGE_RECORDS_MAX];
}fileLookUpResponse;

typedef struct
{
    char fileName[FILE_NAME_LEN_MAX];
}fileUpLoadRequest;

typedef struct
{
    char fileName[FILE_NAME_LEN_MAX];
}fileUpLoadResponse;


typedef struct
{
    char primaryKey[USR_KEY_LNE_MAX + 1];
    char newKey[USR_KEY_LNE_MAX + 1];
}normalUserMgrRequest;

typedef struct
{
    char userName[USR_KEY_LNE_MAX + 1];
    char adminKey[USR_KEY_LNE_MAX + 1];
    char newKey[USR_KEY_LNE_MAX + 1];
}superUserMgrRequest;

typedef struct
{
    time_t currentTime;
}sysTimeGetResponse;

typedef struct
{
    time_t correctTime;
}sysTimeSetRequest;

typedef struct
{
    char adminPasswd[USR_KEY_LNE_MAX];
}adminPasswdConfirmRequest;

typedef struct
{
    char cfgMgrVersion[VERSION_STRING_LEN_MAX + 1];
    char logicVersion[VERSION_STRING_LEN_MAX + 1];
}versionGetResponse;

typedef struct
{
    logType logType;
    logSignificance logSignificance;
    time_t startTime;
    time_t endTime;
    int start;
    int length;
    int draw;
}logLookUpRequest;

typedef struct
{
    int recordsTotal;
    int length;
    int draw;
    logElement elements[PAGE_RECORDS_MAX];
}logLookUpResponse;

typedef struct
{
    logType logType;
    logSignificance logSignificance;
    time_t startTime;
    time_t endTime;
}logExportRequest;

typedef struct
{
    int recordsTotal;
    char logSearchResult[FILE_NAME_LEN_MAX + 1];
}logExportResponse;


#define DISK_INFO_STRING_LEN_MAX 30
typedef struct
{
    char formFactor[DISK_INFO_STRING_LEN_MAX];
    char rate[DISK_INFO_STRING_LEN_MAX];
    char cacheBufferSize[DISK_INFO_STRING_LEN_MAX];
    char modelNumber[DISK_INFO_STRING_LEN_MAX];
    char sn[DISK_INFO_STRING_LEN_MAX];
    char firwareRevision[DISK_INFO_STRING_LEN_MAX];
    char temp[DISK_INFO_STRING_LEN_MAX];
    char size[DISK_INFO_STRING_LEN_MAX];
    char used[DISK_INFO_STRING_LEN_MAX];
    char avail[DISK_INFO_STRING_LEN_MAX];
}diskInfoResponse;

typedef enum
{
    LINK_DOWN = 0,
    LINK_UP
}linkStatus;

typedef struct
{
    linkStatus linkStat;
    int linkSpeed;
    int nRecvPackages;
    int nSendPackages;
}lanStatus;

#define SYSTEM_INFO_STRING_LEN_MAX 30
typedef struct 
{
    char hwVer[SYSTEM_INFO_STRING_LEN_MAX];
    char sn[SYSTEM_INFO_STRING_LEN_MAX];
    char cfgMgrVersion[VERSION_STRING_LEN_MAX + 1];
    time_t cfgMgrLastUpdateTime;
    char logicVersion[VERSION_STRING_LEN_MAX + 1];
    time_t logicLastUpdateTime;
    time_t currentTime;
    int runningSec;
    lanStatus lan1Status;
    lanStatus lan2Status;
}systemInfoResponse;


typedef struct
{
    cfgMgrStatus status;
    char errMessage[LOG_BUF_LEN_MAX + 1];
}confirmResponse;

typedef struct
{
    msgType type;
    char    data[MSG_DATA_LEN_MAX];
}msg;

typedef mqd_t msgID;

msgID msgOpen (char *msgName);

void msgClose (msgID id);

int msgSend (msgID id, msg *m);

int msgRecv (msgID id, msg *m);

#endif

