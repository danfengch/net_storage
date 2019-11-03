/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     share.h
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/10/15
 * @defgroup share
 * @ingroup  cfgMgr cgi  
 * @brief    this method will be used by CfgMgr & CGI
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/10/08    Chenxu        None              File Create
 ****************************************************************************** 
 */

#ifndef __SHARE_H__
#define __SHARE_H__

int macString2Hex (char *str, unsigned char *mac);

int macHex2String (unsigned char *mac, char *str);

/*
 * YYYY-MM-DD HH:MM:SS to time_t
 */
time_t format2time(char *format);

void time2format(time_t ti, char *fmt);

void time2format1(time_t ti, char *fmt);

const char *time2format2(time_t ti);


#endif

