/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     parameters.h
  * @author   Chenxu
  * @version  V1.0.0
  * @date     2019/09/25
  * @defgroup cfgMgr
  * @ingroup  cfgMgr  
  * @brief    Parameters Declare
  * @par History
  * Date          Owner         BugID/CRID        Contents
  * 2019/09/25    Chenxu        None              File Create
  ****************************************************************************** 
  */

#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include <arpa/inet.h>
#include <stdlib.h>

#define USR_KEY_LNE_MAX        20

#ifndef FALSE
#define FALSE  0
#endif

#ifndef TRUE
#define TRUE  1
#endif


typedef struct
{
    int             isDhcp;
    in_addr_t        ip;
    in_addr_t        mask;
    in_addr_t        gateway;
    unsigned char    mac[6];
}netParam;

typedef struct
{
    int             isCapture;
    int             isAutoUpLoad;
    char             autoUpLoadPath[255];
}captureParam;

typedef struct
{
    int             isFilter;
}filterParam;

typedef struct
{
    netParam         net;
    captureParam     capture;
    filterParam      filter;
}lanParam;

typedef struct
{
    char             userName[USR_KEY_LNE_MAX + 1];
    char             passwd[USR_KEY_LNE_MAX + 1];
}userParam;

typedef struct
{
    lanParam         lan1;
    lanParam         lan2;
    userParam        users[2];            /** normal user : user[0], super user : user[1] */
}param;

#endif
