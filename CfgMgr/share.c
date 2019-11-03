/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     share.c
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/10/08
 * @defgroup share
 * @ingroup  cfgMgr cgi  
 * @brief    this method will be used by CfgMgr & CGI
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/10/08    Chenxu        None              File Create
 ****************************************************************************** 
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static char timeFormat[40];
 

int macString2Hex (char *str, unsigned char *mac)
{
    int i;
    int len;
    int temp;
	
    if ((len = strlen(str)) < 17)
		return -1;
	
	for (i = 0; i < 6; i++)
	{
	    sscanf(&str[i * 2 + i], "%02x", &temp);
        mac[i] = temp%256;
	}

	return 0;
}

int macHex2String (unsigned char *mac, char *str)
{
    int i;

	for (i = 0; i < 6; i++)
	{
	    snprintf(&str[i*2 + i], 4, "%02x-", mac[i]);
	}
	str[17] = 0;

	return 0;
}

/*
 * YYYY-MM-DD HH:MM:SS to time_t
 */
time_t format2time(char *format)
{
    struct tm daytime;

	daytime.tm_year = atoi(format) - 1900;
	daytime.tm_mon  = atoi(&format[5]) - 1;
	daytime.tm_mday = atoi(&format[8]);
	daytime.tm_hour = atoi(&format[11]);
	daytime.tm_min = atoi(&format[14]);
	daytime.tm_sec = atoi(&format[17]);

	return (mktime(&daytime));
}

void time2format(time_t ti, char *fmt)
{
    struct tm daytime;
    
    localtime_r(&ti, &daytime);

	sprintf(fmt, "%04d-%02d-%02d %02d:%02d:%02d",
        daytime.tm_year + 1900,
        daytime.tm_mon + 1,
        daytime.tm_mday,
        daytime.tm_hour,
        daytime.tm_min,
        daytime.tm_sec);
}

void time2format1(time_t ti, char *fmt)
{
    struct tm daytime;
    
    localtime_r(&ti, &daytime);

	sprintf(fmt, "%04d%02d%02d%02d%02d%02d",
        daytime.tm_year + 1900,
        daytime.tm_mon + 1,
        daytime.tm_mday,
        daytime.tm_hour,
        daytime.tm_min,
        daytime.tm_sec);
}

const char *time2format2(time_t ti)
{
    struct tm daytime;
    
    localtime_r(&ti, &daytime);

	sprintf(timeFormat, "%04d-%02d-%02d %02d:%02d:%02d",
        daytime.tm_year + 1900,
        daytime.tm_mon + 1,
        daytime.tm_mday,
        daytime.tm_hour,
        daytime.tm_min,
        daytime.tm_sec);

    return (const char *)timeFormat;
}



