/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     web.c
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/09/25
 * @defgroup cfgMgr
 * @ingroup  cfgMgr  
 * @brief    Web Request Function implement
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/09/25    Chenxu        None              File Create
 ****************************************************************************** 
 */
#include <config.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <parameters.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/prctl.h>
#include <cfgMgr.h>
#include <linux/if.h>  
#include <linux/ethtool.h> 
#include <linux/sockios.h>
#include <net/route.h>
#include <net/if_arp.h>
#include <trace.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <message.h>
#include <assert.h>
#include <mxml.h>
#include <version.h>
#include <share.h>
//#include <cfgmgrDumpcapMessage.h>

#define ETHER_ADDR_LEN    6
#define UP    1
#define DOWN    0
#define BCD2HEX(x) (((x) >> 4) * 10 + ((x) & 0x0F))       /*20H -> 20*/
#define HEX2BCD(x) (((x) % 10) + ((((x) / 10) % 10) << 4))  /*20 -> 20H*/
#define MAX(x, y)   (((x) < (y)) ? (y) : (x))
#define MIN(x, y)   (((x) < (y)) ? (x) : (y))





#define WEB_THREAD_NAME "CfgMgrWebThread"
#define CONFIG_FILE_NAME "../cfg/config.xml"

pthread_t webThreadId = -1;

static param pa;
static msgID cfgmgrDumpcapMsgId0;
static msgID cfgmgrDumpcapMsgId1;


static cfgMgrStatus paramLoad(param *p)
{
	int whitespace;
	FILE *fp;
	mxml_node_t *tree,
		*Config,
		*Lan1,*Lan1_Auto,*Lan1_IP,*Lan1_Mask,*Lan1_GateWay,*Lan1_Mac,
			*Lan1_CaptureServiceStatus,*Lan1_AutoUpLoadEnable,*Lan1_AutoUpLoadPath,*Lan1_NetFilterServiceStatus,	
		*Lan2,*Lan2_Auto,*Lan2_IP,*Lan2_Mask,*Lan2_GateWay,*Lan2_Mac,
			*Lan2_CaptureServiceStatus,*Lan2_AutoUpLoadEnable,*Lan2_AutoUpLoadPath,*Lan2_NetFilterServiceStatus,
		*User,*Administrators,*NomalUser,*UserName,*UserPws,
		*node;
	char *attr_value;

    memset(p, 0, sizeof(param));

	if(NULL == (fp = fopen(CONFIG_FILE_NAME, "r")))
	{
		trace(DEBUG_ERR, SYSTEM, "fopen %s failed", CONFIG_FILE_NAME);
		return CFGMGR_ERR;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
	fclose(fp);

	Config = mxmlFindElement(tree, tree, (const char *)"Config", NULL, NULL, MXML_DESCEND);
	assert(Config);

	/** Lan1 */
	Lan1 = mxmlFindElement(Config, Config, (const char *)"Lan1", NULL, NULL, MXML_DESCEND);
	assert(Lan1);
	    /** Lan1_Auto */
	    Lan1_Auto = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_Auto", NULL, NULL, MXML_DESCEND);
		assert(Lan1_Auto);
		node = mxmlGetLastChild(Lan1_Auto);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value,"true"))
			p->lan1.net.isDhcp = TRUE;
		else
			p->lan1.net.isDhcp = FALSE;
		trace(DEBUG_INFO, SYSTEM, "Lan1 dhcp %s", p->lan1.net.isDhcp ? "true" : "false");
		/** Lan1_IP */
	    Lan1_IP = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_IP", NULL, NULL, MXML_DESCEND);
		assert(Lan1_IP);
		node = mxmlGetLastChild(Lan1_IP);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan1.net.ip);
        trace(DEBUG_INFO, SYSTEM, "Lan1_IP 0x%08x", (int)p->lan1.net.ip);
		/** Lan1_Mask */
	    Lan1_Mask = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_Mask", NULL, NULL, MXML_DESCEND);
		assert(Lan1_Mask);
		node = mxmlGetLastChild(Lan1_Mask);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan1.net.mask);
        trace(DEBUG_INFO, SYSTEM, "Lan1_Mask 0x%08x", (int)p->lan1.net.mask);
		/** Lan1_GateWay */
	    Lan1_GateWay = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_GateWay", NULL, NULL, MXML_DESCEND);
		assert(Lan1_GateWay);
		node = mxmlGetLastChild(Lan1_GateWay);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan1.net.gateway);
        trace(DEBUG_INFO, SYSTEM, "Lan1_GateWay 0x%08x", (int)p->lan1.net.gateway);
		/** Lan1_Mac */
	    Lan1_Mac = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_Mac", NULL, NULL, MXML_DESCEND);
		assert(Lan1_Mac);
		node = mxmlGetLastChild(Lan1_Mac);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		macString2Hex(attr_value, p->lan1.net.mac);
        trace(DEBUG_INFO, SYSTEM, "Lan1_Mac %s", attr_value);
		/** Lan1_CaptureServiceStatus */
		Lan1_CaptureServiceStatus = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_CaptureServiceStatus", NULL, NULL, MXML_DESCEND);
		assert(Lan1_CaptureServiceStatus);
		node = mxmlGetLastChild(Lan1_CaptureServiceStatus);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "true"))
			p->lan1.capture.isCapture = TRUE;
		else
			p->lan1.capture.isCapture = FALSE;
        trace(DEBUG_INFO, SYSTEM, "Lan1_CaptureServiceStatus %s", p->lan1.capture.isCapture ? "true":"false");
		/** Lan1_AutoUpLoadEnable */
		Lan1_AutoUpLoadEnable = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_AutoUpLoadEnable", NULL, NULL, MXML_DESCEND);
		assert(Lan1_AutoUpLoadEnable);
		node = mxmlGetLastChild(Lan1_AutoUpLoadEnable);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "true"))
			p->lan1.capture.isCapture = TRUE;
		else
			p->lan1.capture.isCapture = FALSE;
        trace(DEBUG_INFO, SYSTEM, "Lan1_AutoUpLoadEnable %s", p->lan1.capture.isCapture ? "true":"false");
		/** Lan1_AutoUpLoadPath */
		Lan1_AutoUpLoadPath = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_AutoUpLoadPath", NULL, NULL, MXML_DESCEND);
        assert(Lan1_AutoUpLoadPath);        
		node = mxmlGetLastChild(Lan1_AutoUpLoadPath);
		if(node)
        {
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		strncpy(p->lan1.capture.autoUpLoadPath, attr_value, sizeof(p->lan1.capture.autoUpLoadPath));
        }
//        trace(DEBUG_INFO, SYSTEM, "Lan1_AutoUpLoadPath %s", p->lan1.capture.autoUpLoadPath);
		/** Lan1_NetFilterServiceStatus */
		Lan1_NetFilterServiceStatus = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_NetFilterServiceStatus", NULL, NULL, MXML_DESCEND);
		assert(Lan1_NetFilterServiceStatus);
		node = mxmlGetLastChild(Lan1_NetFilterServiceStatus);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "true"))
			p->lan1.filter.isFilter = TRUE;
		else
			p->lan1.filter.isFilter = FALSE;
        trace(DEBUG_INFO, SYSTEM, "Lan1_CaptureServiceStatus %s", p->lan1.filter.isFilter ? "true":"false");
		
						
	/** Lan2 */
    Lan2 = mxmlFindElement(Config, Config, (const char *)"Lan2", NULL, NULL, MXML_DESCEND);
	assert(Lan2);
	    /** Lan2_Auto */
	    Lan2_Auto = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_Auto", NULL, NULL, MXML_DESCEND);
		assert(Lan2_Auto);
		node = mxmlGetLastChild(Lan2_Auto);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value,"true"))
			p->lan2.net.isDhcp = TRUE;
		else
			p->lan2.net.isDhcp = FALSE;
		trace(DEBUG_INFO, SYSTEM, "Lan2 dhcp %s", p->lan2.net.isDhcp ? "true" : "false");
		/** Lan2_IP */
	    Lan2_IP = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_IP", NULL, NULL, MXML_DESCEND);
		assert(Lan2_IP);
		node = mxmlGetLastChild(Lan2_IP);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan2.net.ip);
        trace(DEBUG_INFO, SYSTEM, "Lan2_IP 0x%08x", (int)p->lan2.net.ip);
		/** Lan2_Mask */
	    Lan2_Mask = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_Mask", NULL, NULL, MXML_DESCEND);
		assert(Lan2_Mask);
		node = mxmlGetLastChild(Lan2_Mask);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan2.net.mask);
        trace(DEBUG_INFO, SYSTEM, "Lan2_Mask 0x%08x", (int)p->lan2.net.mask);
		/** Lan2_GateWay */
	    Lan2_GateWay = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_GateWay", NULL, NULL, MXML_DESCEND);
		assert(Lan2_GateWay);
		node = mxmlGetLastChild(Lan2_GateWay);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan2.net.gateway);
        trace(DEBUG_INFO, SYSTEM, "Lan2_GateWay 0x%08x", (int)p->lan2.net.gateway);
		/** Lan2_Mac */
	    Lan2_Mac = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_Mac", NULL, NULL, MXML_DESCEND);
		assert(Lan2_Mac);
		node = mxmlGetLastChild(Lan2_Mac);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		macString2Hex(attr_value, p->lan2.net.mac);
        trace(DEBUG_INFO, SYSTEM, "Lan2_Mac %s", attr_value);
		/** Lan2_CaptureServiceStatus */
		Lan2_CaptureServiceStatus = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_CaptureServiceStatus", NULL, NULL, MXML_DESCEND);
		assert(Lan2_CaptureServiceStatus);
		node = mxmlGetLastChild(Lan2_CaptureServiceStatus);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "true"))
			p->lan2.capture.isCapture = TRUE;
		else
			p->lan2.capture.isCapture = FALSE;
        trace(DEBUG_INFO, SYSTEM, "Lan2_CaptureServiceStatus %s", p->lan2.capture.isCapture ? "true":"false");
		/** Lan2_AutoUpLoadEnable */
		Lan2_AutoUpLoadEnable = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_AutoUpLoadEnable", NULL, NULL, MXML_DESCEND);
		assert(Lan2_AutoUpLoadEnable);
		node = mxmlGetLastChild(Lan2_AutoUpLoadEnable);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "true"))
			p->lan2.capture.isCapture = TRUE;
		else
			p->lan2.capture.isCapture = FALSE;
        trace(DEBUG_INFO, SYSTEM, "Lan2_AutoUpLoadEnable %s", p->lan2.capture.isCapture ? "true":"false");
		/** Lan2_AutoUpLoadPath */
		Lan2_AutoUpLoadPath = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_AutoUpLoadPath", NULL, NULL, MXML_DESCEND);
        assert(Lan2_AutoUpLoadPath);        
		node = mxmlGetLastChild(Lan2_AutoUpLoadPath);
		if(node)
        {
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		strncpy(p->lan2.capture.autoUpLoadPath, attr_value, sizeof(p->lan2.capture.autoUpLoadPath));
        }
//        trace(DEBUG_INFO, SYSTEM, "Lan2_AutoUpLoadPath %s", p->lan2.capture.autoUpLoadPath);
		/** Lan2_NetFilterServiceStatus */
		Lan2_NetFilterServiceStatus = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_NetFilterServiceStatus", NULL, NULL, MXML_DESCEND);
		assert(Lan2_NetFilterServiceStatus);
		node = mxmlGetLastChild(Lan2_NetFilterServiceStatus);
		assert(node);
		attr_value = (char * )mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "true"))
			p->lan2.filter.isFilter = TRUE;
		else
			p->lan2.filter.isFilter = FALSE;
        trace(DEBUG_INFO, SYSTEM, "Lan2_CaptureServiceStatus %s", p->lan2.filter.isFilter ? "true":"false");
	
	/** User */
	User = mxmlFindElement(Config, Config, (const char *)"User", NULL, NULL, MXML_DESCEND);
	assert(User);
	    /** Administrators */
	    Administrators = mxmlFindElement(User, User, (const char *)"Administrators", NULL, NULL, MXML_DESCEND);
		assert(Administrators);
		    /** UserName */
		    UserName = mxmlFindElement(Administrators, Administrators, (const char *)"UserName", NULL, NULL, MXML_DESCEND);
			assert(UserName);
			node = mxmlGetLastChild(UserName);
			assert(node);
			attr_value = (char * )mxmlGetText(node, &whitespace);
			assert(attr_value);
			strncpy(p->users[0].userName, attr_value, USR_KEY_LNE_MAX + 1);
	        trace(DEBUG_INFO, SYSTEM, "Administrators UserName %s", p->users[0].userName);
			/** UserPws */
		    UserPws = mxmlFindElement(Administrators, Administrators, (const char *)"UserPws", NULL, NULL, MXML_DESCEND);
			assert(UserPws);
			node = mxmlGetLastChild(UserPws);
			assert(node);
			attr_value = (char * )mxmlGetText(node, &whitespace);
			assert(attr_value);
			strncpy(p->users[0].passwd, attr_value, USR_KEY_LNE_MAX + 1);
	        trace(DEBUG_INFO, SYSTEM, "Administrators UserPws %s", p->users[0].passwd);	    
        /** NomalUser */
	    NomalUser = mxmlFindElement(User, User, (const char *)"NomalUser", NULL, NULL, MXML_DESCEND);
		assert(NomalUser);
		    /** UserName */
		    UserName = mxmlFindElement(NomalUser, NomalUser, (const char *)"UserName", NULL, NULL, MXML_DESCEND);
			assert(UserName);
			node = mxmlGetLastChild(UserName);
			assert(node);
			attr_value = (char * )mxmlGetText(node, &whitespace);
			assert(attr_value);
			strncpy(p->users[1].userName, attr_value, USR_KEY_LNE_MAX + 1);
	        trace(DEBUG_INFO, SYSTEM, "NomalUser UserName %s", p->users[1].userName);
			/** UserPws */
		    UserPws = mxmlFindElement(NomalUser, NomalUser, (const char *)"UserPws", NULL, NULL, MXML_DESCEND);
			assert(UserPws);
			node = mxmlGetLastChild(UserPws);
			assert(node);
			attr_value = (char * )mxmlGetText(node, &whitespace);
			assert(attr_value);
			strncpy(p->users[1].passwd, attr_value, USR_KEY_LNE_MAX + 1);
	        trace(DEBUG_INFO, SYSTEM, "NomalUser UserPws %s", p->users[1].passwd);
			
	mxmlDelete(tree);

	return CFGMGR_OK;
}

static cfgMgrStatus paramSave (param *p)
{
	cfgMgrStatus ret = CFGMGR_ERR;
	char buffer[100] = {0};
	FILE		*fp;
	mxml_node_t *tree, 
		*Config,
		*Lan1,*Lan1_Auto,*Lan1_IP,*Lan1_Mask,*Lan1_GateWay,*Lan1_Mac,
		    *Lan1_CaptureServiceStatus,*Lan1_AutoUpLoadEnable,*Lan1_AutoUpLoadPath,*Lan1_NetFilterServiceStatus,	
		*Lan2,*Lan2_Auto,*Lan2_IP,*Lan2_Mask,*Lan2_GateWay,*Lan2_Mac,
		    *Lan2_CaptureServiceStatus,*Lan2_AutoUpLoadEnable,*Lan2_AutoUpLoadPath,*Lan2_NetFilterServiceStatus,	
		*User,*Administrators,*NomalUser,*UserName,*UserPws,
		*node;

	tree = mxmlNewXML("1.0");
	assert(tree);
	Config = mxmlNewElement(tree, "Config");
	assert(Config);
		Lan1 = mxmlNewElement(Config, "Lan1");
		assert(Lan1);
		    /** Lan1_Auto */
			Lan1_Auto = mxmlNewElement(Lan1, "Lan1_Auto");
			assert(Lan1_Auto);
			node = mxmlNewText(Lan1_Auto, 0, p->lan1.net.isDhcp ? "true" : "false");
			assert(node);
			/** Lan1_IP */
			Lan1_IP = mxmlNewElement(Lan1, "Lan1_IP");
			assert(Lan1_IP);
			inet_ntop(AF_INET, (void *)&p->lan1.net.ip, buffer, 16);
			node = mxmlNewText(Lan1_IP, 0, buffer);
			assert(node);
			/** Lan1_Mask */
			Lan1_Mask = mxmlNewElement(Lan1, "Lan1_Mask");
			assert(Lan1_Mask);
			inet_ntop(AF_INET, (void *)&p->lan1.net.mask, buffer, 16);
			node = mxmlNewText(Lan1_Mask, 0, buffer);
			assert(node);
			/** Lan1_GateWay */
			Lan1_GateWay = mxmlNewElement(Lan1, "Lan1_GateWay");
			assert(Lan1_GateWay);
			inet_ntop(AF_INET, (void *)&p->lan1.net.gateway, buffer, 16);
			node = mxmlNewText(Lan1_GateWay, 0, buffer);
			assert(node);
			/** Lan1_Mac */
			Lan1_Mac = mxmlNewElement(Lan1, "Lan1_Mac");
			assert(Lan1_Mac);
			macHex2String(p->lan1.net.mac, buffer);
			node = mxmlNewText(Lan1_Mac, 0, buffer);
			assert(node);
			/** Lan1_CaptureServiceStatus */
			Lan1_CaptureServiceStatus = mxmlNewElement(Lan1, "Lan1_CaptureServiceStatus");
			assert(Lan1_CaptureServiceStatus);
			if(p->lan1.capture.isCapture)
				strncpy(buffer, "true", sizeof(buffer));
			else
				strncpy(buffer, "false", sizeof(buffer));
			node = mxmlNewText(Lan1_CaptureServiceStatus, 0, buffer);
			assert(node);
			/** Lan1_AutoUpLoadEnable */
			Lan1_AutoUpLoadEnable = mxmlNewElement(Lan1, "Lan1_AutoUpLoadEnable");
			assert(Lan1_AutoUpLoadEnable);
			if(p->lan1.capture.isAutoUpLoad)
				strncpy(buffer, "true", sizeof(buffer));
			else
				strncpy(buffer, "false", sizeof(buffer));
			node = mxmlNewText(Lan1_AutoUpLoadEnable, 0, buffer);
			assert(node);
			/** Lan1_AutoUpLoadPath */
			Lan1_AutoUpLoadPath = mxmlNewElement(Lan1, "Lan1_AutoUpLoadPath");
			assert(Lan1_AutoUpLoadPath);
			node = mxmlNewText(Lan1_AutoUpLoadPath, 0, p->lan1.capture.autoUpLoadPath);
			assert(node);
			/** Lan1_NetFilterServiceStatus */
			Lan1_NetFilterServiceStatus = mxmlNewElement(Lan1, "Lan1_NetFilterServiceStatus");
			assert(Lan1_NetFilterServiceStatus);
			if(p->lan1.filter.isFilter)
				strncpy(buffer, "true", sizeof(buffer));
			else
				strncpy(buffer, "false", sizeof(buffer));
			node = mxmlNewText(Lan1_NetFilterServiceStatus, 0, buffer);
			assert(node);
			
			
	    Lan2 = mxmlNewElement(Config, "Lan2");
		assert(Lan2);
		    /** Lan2_Auto */
			Lan2_Auto = mxmlNewElement(Lan2, "Lan2_Auto");
			assert(Lan2_Auto);
			node = mxmlNewText(Lan2_Auto, 0, p->lan2.net.isDhcp ? "true" : "false");
			assert(node);
			/** Lan2_IP */
			Lan2_IP = mxmlNewElement(Lan2, "Lan2_IP");
			assert(Lan2_IP);
			inet_ntop(AF_INET, (void *)&p->lan2.net.ip, buffer, 26);
			node = mxmlNewText(Lan2_IP, 0, buffer);
			assert(node);
			/** Lan2_Mask */
			Lan2_Mask = mxmlNewElement(Lan2, "Lan2_Mask");
			assert(Lan2_Mask);
			inet_ntop(AF_INET, (void *)&p->lan2.net.mask, buffer, 26);
			node = mxmlNewText(Lan2_Mask, 0, buffer);
			assert(node);
			/** Lan2_GateWay */
			Lan2_GateWay = mxmlNewElement(Lan2, "Lan2_GateWay");
			assert(Lan2_GateWay);
			inet_ntop(AF_INET, (void *)&p->lan2.net.gateway, buffer, 26);
			node = mxmlNewText(Lan2_GateWay, 0, buffer);
			assert(node);
			/** Lan2_Mac */
			Lan2_Mac = mxmlNewElement(Lan2, "Lan2_Mac");
			assert(Lan2_Mac);
			macHex2String(p->lan2.net.mac, buffer);
			node = mxmlNewText(Lan2_Mac, 0, buffer);
			assert(node);
			/** Lan2_CaptureServiceStatus */
			Lan2_CaptureServiceStatus = mxmlNewElement(Lan2, "Lan2_CaptureServiceStatus");
			assert(Lan2_CaptureServiceStatus);
			if(p->lan2.capture.isCapture)
				strncpy(buffer, "true", sizeof(buffer));
			else
				strncpy(buffer, "false", sizeof(buffer));
			node = mxmlNewText(Lan2_CaptureServiceStatus, 0, buffer);
			assert(node);
			/** Lan2_AutoUpLoadEnable */
			Lan2_AutoUpLoadEnable = mxmlNewElement(Lan2, "Lan2_AutoUpLoadEnable");
			assert(Lan2_AutoUpLoadEnable);
			if(p->lan2.capture.isAutoUpLoad)
				strncpy(buffer, "true", sizeof(buffer));
			else
				strncpy(buffer, "false", sizeof(buffer));
			node = mxmlNewText(Lan2_AutoUpLoadEnable, 0, buffer);
			assert(node);
			/** Lan2_AutoUpLoadPath */
			Lan2_AutoUpLoadPath = mxmlNewElement(Lan2, "Lan2_AutoUpLoadPath");
			assert(Lan2_AutoUpLoadPath);
			node = mxmlNewText(Lan2_AutoUpLoadPath, 0, p->lan2.capture.autoUpLoadPath);
			assert(node);
			/** Lan2_NetFilterServiceStatus */
			Lan2_NetFilterServiceStatus = mxmlNewElement(Lan2, "Lan2_NetFilterServiceStatus");
			assert(Lan2_NetFilterServiceStatus);
			if(p->lan2.filter.isFilter)
				strncpy(buffer, "true", sizeof(buffer));
			else
				strncpy(buffer, "false", sizeof(buffer));
			node = mxmlNewText(Lan2_NetFilterServiceStatus, 0, buffer);
		/** User */
		User = mxmlNewElement(Config, "User");
		assert(User);
		    /** Administrators */
		    Administrators = mxmlNewElement(User, "Administrators");
			assert(Administrators);
			    /** UserName */
			    UserName = mxmlNewElement(Administrators, "UserName");
				assert(UserName);
				node = mxmlNewText(UserName, 0, p->users[0].userName);
				assert(node);
				/** UserPws */
			    UserPws = mxmlNewElement(Administrators, "UserPws");
				assert(UserPws);
				node = mxmlNewText(UserPws, 0, p->users[0].passwd);
				assert(node);
			/** NomalUser */
		    NomalUser = mxmlNewElement(User, "NomalUser");
			assert(NomalUser);
			    /** UserName */
			    UserName = mxmlNewElement(NomalUser, "UserName");
				assert(UserName);
				node = mxmlNewText(UserName, 0, p->users[1].userName);
				assert(node);
				/** UserPws */
			    UserPws = mxmlNewElement(NomalUser, "UserPws");
				assert(UserPws);
				node = mxmlNewText(UserPws, 0, p->users[1].passwd);
				assert(node);

	if(NULL == (fp = fopen(CONFIG_FILE_NAME, "w+")))
		goto Error;

	assert(mxmlSaveFile(tree, fp, MXML_NO_CALLBACK) != -1);
	fclose(fp);
	ret =  CFGMGR_OK;

Error:
	mxmlDelete(tree);
	return ret;
}


static int is_netipvalid( in_addr_t IP )
{
	int i;
	struct in_addr addr;
	addr.s_addr = IP;

	i = inet_addr(inet_ntoa(addr));

	if((i == 0)||(i == 0xffffffff))
		return FALSE;
	else
		return TRUE;
}

static cfgMgrStatus set_addr( in_addr_t addr, int flag, int ethn)
{
    struct ifreq ifr;
    struct sockaddr_in sin;
	struct in_addr test;
    int sockfd;

	test.s_addr = addr;
	if(!is_netipvalid(addr))
	{
	    trace(DEBUG_ERR, USER, "invalid IP[%s]!!!", inet_ntoa(test));
		return CFGMGR_IP_INVALID;
	}
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
	{
        trace(DEBUG_ERR, SYSTEM, "socket fail!!!");
        return CFGMGR_ERR;
    }

	if(1 == ethn)
	    snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", NET1_NAME);
	else if(2 == ethn)
		snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", NET2_NAME);
    else
	{
	    trace(DEBUG_ERR, USER, "netNumber [%d] is invalid!!!", ethn);
        return CFGMGR_NET_NUMBER_INVALID;
	}
    
	/* Read interface flags */
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
		trace(DEBUG_ERR, SYSTEM, "ifdown: shutdown ");

    memset(&sin, 0, sizeof(struct sockaddr));

    sin.sin_family = AF_INET;

    sin.sin_addr.s_addr = addr;

    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));

	if(ioctl(sockfd, flag, &ifr) < 0)
	{
		trace(DEBUG_ERR, SYSTEM, "fail to set address [%s]. ", inet_ntoa(test));
        close(sockfd);
		return CFGMGR_ERR;
	}

    close(sockfd);

    return CFGMGR_OK;
}

static cfgMgrStatus set_gateway( in_addr_t addr, int ethn )
{
#if 1
    char cmd[200] = {0};
    char buffer[20] = {0};

    inet_ntop(AF_INET, (void *)&addr, buffer, 16);
    snprintf(cmd, sizeof(cmd), "route add default gw %s", buffer);
    system(cmd);
    return CFGMGR_OK;
#else
    int sockFd;
//    struct sockaddr_in sockaddr;
    struct rtentry rt;
	int ret;

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockFd < 0)
	{
		trace(DEBUG_ERR, SYSTEM, "set_gateway Socket create error.");
		return CFGMGR_ERR;
	}

    /* Delete existing defalt gateway */    
    memset(&rt, 0, sizeof(struct rtentry));
    rt.rt_dst.sa_family = AF_INET; 
    rt.rt_genmask.sa_family = AF_INET; 
	rt.rt_flags = RTF_UP; 
	if ((ret = ioctl(sockFd, SIOCDELRT, &rt)) < 0)
	{
		 trace(DEBUG_ERR, SYSTEM, "ioctl(SIOCDELRT) error[%d]\n", ret);
		 close(sockFd);
		 return CFGMGR_ERR;
	}

	memset(&rt, 0, sizeof(struct rtentry));
    rt.rt_dst.sa_family = AF_INET; 
    ((struct sockaddr_in *)&rt.rt_gateway)->sin_family = AF_INET;    
	((struct sockaddr_in *)&rt.rt_gateway)->sin_addr.s_addr = addr; 

	((struct sockaddr_in *)&rt.rt_dst)->sin_family = AF_INET;
	
	((struct sockaddr_in *)&rt.rt_genmask)->sin_family = AF_INET;
    rt.rt_flags = RTF_GATEWAY;

	if ((ret = ioctl(sockFd, SIOCADDRT, &rt)) < 0)
	{
		struct in_addr inaddr;

		inaddr.s_addr = addr;
		trace(DEBUG_ERR, SYSTEM, "ioctl(SIOCADDRT) [%s] error[%d]", inet_ntoa(inaddr), ret);
		close(sockFd);
		return CFGMGR_ERR;
	}

	return CFGMGR_OK;
#endif
}

int get_mac_addr(char *ifname, char *mac)
{
    int fd, rtn;
    struct ifreq ifr;
    
    if( !ifname || !mac ) {
        return -1;
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 ) {
        perror("socket");
           return -1;
    }
    ifr.ifr_addr.sa_family = AF_INET;    
    strncpy(ifr.ifr_name, (const char *)ifname, IFNAMSIZ - 1 );

    if ( (rtn = ioctl(fd, SIOCGIFHWADDR, &ifr) ) == 0 )
        memcpy(mac, (unsigned char *)ifr.ifr_hwaddr.sa_data, 6);
    close(fd);
    return rtn;
}

int
ether_atoe(const char *a, unsigned char *e)
{
    char *c = (char *) a;
    int i = 0;

    memset(e, 0, ETHER_ADDR_LEN);
    for (;;) {
        e[i++] = (unsigned char) strtoul(c, &c, 16);
        if (!*c++ || i == ETHER_ADDR_LEN)
            break;
    }
    return (i == ETHER_ADDR_LEN);
}

char *
ether_etoa(const unsigned char *e, char *a)
{
    char *c = a;
    int i;

    for (i = 0; i < ETHER_ADDR_LEN; i++) {
        if (i)
            *c++ = ':';
        c += sprintf(c, "%02X", e[i] & 0xff);
    }
    return a;
}


int if_updown(char *ifname, int flag)
{
    int fd, rtn;
    struct ifreq ifr;

    if (!ifname) {
        return -1;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 ) {
        perror("socket");
        return -1;
    }
    
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, (const char *)ifname, IFNAMSIZ - 1 );

    if ( (rtn = ioctl(fd, SIOCGIFFLAGS, &ifr) ) == 0 ) {
        if ( flag == DOWN )
            ifr.ifr_flags &= ~IFF_UP;
        else if ( flag == UP ) 
            ifr.ifr_flags |= IFF_UP;
    }

    if ( (rtn = ioctl(fd, SIOCSIFFLAGS, &ifr) ) != 0) {
        perror("SIOCSIFFLAGS");
    }

    if (flag == UP)
    {
        struct ethtool_value edata; 
	
    	edata.cmd = ETHTOOL_GLINK; 
    	edata.data = 0; 
        ifr.ifr_data = (char *) &edata; 
        
        do
        {
            if(((rtn = ioctl( fd, SIOCETHTOOL, &ifr )) == -1) || (edata.data > 0))
            {
                break;
            }            
        }while(1);
    }

    close(fd);

    return rtn;
}


static cfgMgrStatus setMacAddress(int netNumber, unsigned char *mac)
{
    int fd, rtn;
    struct ifreq ifr;
	char ifname[20];

    if (netNumber == 1)
        strncpy(ifname, NET1_NAME, sizeof(ifname));
    else if (netNumber == 2)
        strncpy(ifname, NET2_NAME, sizeof(ifname));
    else
    {
        trace(DEBUG_ERR, USER, "setMacAddress : netNumber %d invalid !!!", netNumber);
        return CFGMGR_ERR;
    }
	
    fd = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 ) 
	{
        trace(DEBUG_ERR, SYSTEM, "setMacAddress : Socket create error.");
		return CFGMGR_ERR;
    }
    ifr.ifr_addr.sa_family = ARPHRD_ETHER;
    strncpy(ifr.ifr_name, (const char *)ifname, IFNAMSIZ - 1 );
    memcpy((unsigned char *)ifr.ifr_hwaddr.sa_data, mac, 6);
    
    if((rtn = ioctl(fd, SIOCSIFHWADDR, &ifr) ) != 0)
    {
        trace(DEBUG_ERR, SYSTEM, "setMacAddress : Set Mac Address(SIOCSIFHWADDR) error(%d).", rtn);
        trace(DEBUG_ERR, SYSTEM, "setMacAddress : Mac %02x %02x %02x %02x %02x %02x.", 
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		return CFGMGR_ERR;
    }
    close(fd);
    return CFGMGR_OK;
}

static cfgMgrStatus setNetParameters(netParam *net, int netNumber)
{
    cfgMgrStatus status = CFGMGR_OK;
    char buffer[100];
    
    /** set ip */
#if 0
    char cmd[100];
    char buffer[100];
    char *netName;
    
    inet_ntop(AF_INET, (void *)&net->ip, buffer, 100);
    if(netNumber == 1)
        netName = NET1_NAME;
    else
        netName = NET2_NAME;

    snprintf(cmd, sizeof(cmd), "ifconfig %s %s", netName, buffer);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "ifconfig %s down", netName);
    system(cmd);    
    snprintf(cmd, sizeof(cmd), "ifconfig %s up", netName);
    system(cmd);


#else
    if(CFGMGR_OK != (status = set_addr(net->ip, SIOCSIFADDR, netNumber)))
    {
        trace(DEBUG_ERR, USER, "net%d setNetParameters : set ip failed !!!", netNumber);
        return status;
    }

    /** set net mask */
    if(CFGMGR_OK != (status = set_addr(net->mask, SIOCSIFNETMASK, netNumber)))
    {
        trace(DEBUG_ERR, USER, "net%d setNetParameters : set net mask failed !!!", netNumber);
        return status;
    }
    /** set gateway */
    if(CFGMGR_OK != (status = set_gateway(net->gateway, netNumber)))
    {
        trace(DEBUG_ERR, USER, "net%d setNetParameters : set gateway failed !!!", netNumber);
        return status;
    }
    get_mac_addr(netNumber == 1 ? NET1_NAME : NET2_NAME, buffer);
    if (memcmp(buffer, net->mac, 6) != 0)
    {
        if_updown(netNumber == 1 ? NET1_NAME : NET2_NAME, DOWN);
        /** set mac */
        if(CFGMGR_OK != (status = setMacAddress(netNumber, net->mac)))
        {
            trace(DEBUG_ERR, USER, "net%d setNetParameters : set mac address failed !!!", netNumber);
            return status;
        }
        if_updown(netNumber == 1 ? NET1_NAME : NET2_NAME, UP);
    }
#endif

    return status;
}

static void genConfirmMsg(cfgMgrStatus status, msg *m)
{
    confirmResponse *resp = (confirmResponse *)m->data;

    memset(m, 0, sizeof(msg));
    
    m->type = MSGTYPE_COMFIRM;
    resp->status = status;
    if(status != CFGMGR_OK)
        strncpy(&resp->errMessage[0], getLastCfgMgrErr(), LOG_BUF_LEN_MAX);
    else
        resp->errMessage[0] = 0;
}

static cfgMgrStatus doLogin(msg* in, msg *out)
{
    int i;
    loginRequest *req = (loginRequest *)in->data; 
    cfgMgrStatus status = CFGMGR_ERR;    

    trace(DEBUG_INFO, USER, "Login start");

    for (i = 0; i < USER_NUM; i++)
    {
        if ((strcmp(pa.users[i].userName, req->userName) == 0)
            && (strcmp(pa.users[i].passwd, req->passwd) == 0))
        {
            status = CFGMGR_OK;
            break;
        }
    }

    if (i >= USER_NUM)
    {
        trace(DEBUG_INFO, USER, "UserName or Passwd not valid !!!");
        goto doLoginExit;
    }
    
    trace(DEBUG_INFO, USER, "Login succ");
    
doLoginExit:

    genConfirmMsg(status, out);
    
    return status;
}

extern int pingTest(in_addr_t);

static cfgMgrStatus doLanTest (msg* in, msg *out, int netNumber)
{
    lanTestRequest *req = (lanTestRequest *)in->data;
    netParam *net = &req->net;
    cfgMgrStatus status = CFGMGR_OK;
    netParam *netOrigin;

    trace(DEBUG_INFO, USER, "Lan %d test start", netNumber);
//#if 0
    /** set net parameters */
    if(CFGMGR_OK != (status = setNetParameters(net, netNumber)))
    {
        trace(DEBUG_ERR, USER, "Lan %d test : setNetParameters failed !!!", netNumber);
        goto doLanTestExit;
    }
     
    /** lan test */
    if (0 != pingTest(req->destIp))
    {
        struct in_addr destIp;
        destIp.s_addr = req->destIp;
        trace(DEBUG_ERR, USER, "Lan %d test : pingTest failed, destIp %s!!!,%x", netNumber, inet_ntoa(destIp), req->destIp);
        status = CFGMGR_NETWORK_UNREACHABLE;
    }

	/** net parameter set bak */
    if (netNumber == 1)
        netOrigin = &pa.lan1.net;
    else
        netOrigin = &pa.lan2.net;
    if(CFGMGR_OK != (status = setNetParameters(netOrigin, netNumber)))
    {
        trace(DEBUG_ERR, USER, "Lan %d test : setNetParameters failed !!!", netNumber);
        goto doLanTestExit;
    }
//#endif    
    trace(DEBUG_INFO, USER, "Lan %d test OK", netNumber);
    
doLanTestExit:

    genConfirmMsg(status, out);
    
    return status;
}

static cfgMgrStatus doNetConfigSave (msg* in, msg *out)
{
    netParam *net = (netParam *)in->data; 
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "Net config start");

    if(CFGMGR_OK != (status = setNetParameters(net, 1)))
    {
        trace(DEBUG_ERR, USER, "net 1 setNetParameters failed.");
        goto doNetConfigSaveExit;
    }
    
    memcpy (&pa.lan1.net, net, sizeof(netParam));

    net++;    
    if(CFGMGR_OK != (status = setNetParameters(net, 2)))
    {
        trace(DEBUG_ERR, USER, "net 2 setNetParameters failed.");
        goto doNetConfigSaveExit;
    }
    
    memcpy (&pa.lan2.net, net, sizeof(netParam));

    if(CFGMGR_OK != (status = paramSave(&pa)))
    {
        trace(DEBUG_ERR, USER, "paramSave failed.");
        goto doNetConfigSaveExit;
    }
    
    trace(DEBUG_INFO, USER, "Net config succ");
    
doNetConfigSaveExit:

    genConfirmMsg(status, out);
    
    return status;
}


static cfgMgrStatus netCapture(captureParam *capture, int netNumber)
{
    //TODO
    return CFGMGR_NOT_SUPPORT;
}

static cfgMgrStatus doNetCapture(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    captureParam *capture = (captureParam *)in->data;

    trace(DEBUG_INFO, USER, "Net Capture start");

//    if(CFGMGR_OK != (status = netCapture(capture, 1)))
//    {
//        trace(DEBUG_ERR, "net 1 netCapture failed.");
//        goto doNetCaptureExit;
//    }
    
    memcpy(&pa.lan1.capture, capture, sizeof(captureParam));

    capture++;
//    if(CFGMGR_OK != (status = netCapture(capture, 2)))
//    {
//        trace(DEBUG_ERR, "net 2 netCapture failed.");
//        goto doNetCaptureExit;
//    }
    
    memcpy(&pa.lan2.capture, capture, sizeof(captureParam));

    if(CFGMGR_OK != (status = paramSave(&pa)))
    {
        trace(DEBUG_ERR, USER, "paramSave failed.");
        goto doNetCaptureExit;
    }

    trace(DEBUG_INFO, USER, "Net Capture succ");

doNetCaptureExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus netFilter(filterParam *filter, int netNumber)
{
    //TODO
    return CFGMGR_NOT_SUPPORT;
}

static cfgMgrStatus doNetFilter(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    filterParam *filter = (filterParam *)in->data;

    trace(DEBUG_INFO, USER, "Net Filter start");

//    if(CFGMGR_OK != (status = netFilter(filter, 1)))
//    {
//        trace(DEBUG_ERR, "net 1 netFilter failed.");
//        goto doNetFilterExit;
//    }
    
    memcpy(&pa.lan1.filter, filter, sizeof(filterParam));

    filter++;
//    if(CFGMGR_OK != (status = netFilter(filter, 2)))
//    {
//        trace(DEBUG_ERR, "net 2 netFilter failed.");
//        goto doNetFilterExit;
//    }
    
    memcpy(&pa.lan2.filter, filter, sizeof(filterParam));

    if(CFGMGR_OK != (status = paramSave(&pa)))
    {
        trace(DEBUG_ERR, USER, "paramSave failed.");
        goto doNetFilterExit;
    }

    trace(DEBUG_INFO, USER, "Net Filter succ");

doNetFilterExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doFileLookUp(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    fileLookUpRequest *req = (fileLookUpRequest *)in->data;
    fileLookUpResponse * resp = (fileLookUpResponse *)out->data;
    int recordsTotal, i, len;
    msgID mId;
    msg *sendMsg = in, *recvMsg = out;

    trace(DEBUG_INFO, USER, "File Look Up start");

    memset(out, 0, sizeof(msg));

    /** file look up */
#if 1
    if (req->netNumber == 1)
    {
        mId = cfgmgrDumpcapMsgId0;
    }
    else
    {
        mId = cfgmgrDumpcapMsgId1;
    }
    
    if(-1 == msgSend(mId, sendMsg))
    {
        trace(DEBUG_ERR, SYSTEM, "msgSend failed !!!");
        goto doNetFilterExit;
    }
    if((len = msgRecv(mId, recvMsg)) <= 0)
    {
        trace(DEBUG_ERR, SYSTEM, "msgRecv %s error !!!", req->netNumber == 1 ? CFGMGR_DUMPCAP_MSG0_NAME : CFGMGR_DUMPCAP_MSG1_NAME);
        goto doNetFilterExit;
    }
#else
{
    time_t ti;
    char timeFmt[30];

    recordsTotal = 1000;
    for (i = 0, ti = req->startTime + req->start; (i < recordsTotal) && (i < req->length); i++, ti++)
    {
        time2format1(ti, timeFmt);
        snprintf(resp->elements[i].fileName, sizeof(resp->elements[i].fileName), "%s", timeFmt);
        resp->elements[i].modifyTime = ti;
        resp->elements[i].sizeMB = 10;
    }
    resp->draw = req->draw;
    resp->recordsTotal = recordsTotal;
    resp->length= i;
}
    out->type = MSGTYPE_FILELOOKUP_RESPONSE;
#endif

    trace(DEBUG_INFO, USER, "File Look Up succ");

doNetFilterExit:

    return status;
}

static cfgMgrStatus doFileUpLoad(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    fileUpLoadRequest *req = (fileUpLoadRequest *)in->data;
    fileUpLoadResponse * resp = (fileUpLoadResponse *)out->data;

    trace(DEBUG_INFO, USER, "File Up Load start");

    memcpy(resp->fileName, req->fileName, FILE_NAME_LEN_MAX);

    out->type = MSGTYPE_FILEUPLOAD_RESPONSE;
    trace(DEBUG_INFO, USER, "File Up Load succ");

    return status;
}


static cfgMgrStatus doNormalUserMgr(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    normalUserMgrRequest *req = (normalUserMgrRequest *)in->data;

    trace(DEBUG_INFO, USER, "Normal User Manage start");

    memset(out, 0, sizeof(msg));

    if (strcmp(pa.users[1].passwd, req->primaryKey) != 0)
    {
        trace(DEBUG_ERR, USER, "Passwd invalid !!!");
        status = CFGMGR_PASSWD_INVALID;
        goto doNormalUserMgrExit;
    }

    if (strlen(req->newKey) > USR_KEY_LNE_MAX)
    {
        trace(DEBUG_ERR, USER, "New Passwd len %d > %d !!!", strlen(req->newKey), USR_KEY_LNE_MAX);
        status = CFGMGR_PASSWD_INVALID;
        goto doNormalUserMgrExit;
    }

    strncpy (pa.users[1].passwd, req->newKey, sizeof(pa.users[1].passwd));

    if(CFGMGR_OK != (status = paramSave(&pa)))
    {
        trace(DEBUG_ERR, USER, "paramSave failed.");
        goto doNormalUserMgrExit;
    }

    trace(DEBUG_INFO, USER, "Normal User Manage succ");

doNormalUserMgrExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doSuperUserMgr(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    superUserMgrRequest *req = (superUserMgrRequest *)in->data;

    trace(DEBUG_INFO, USER, "Super User Manage start");

    memset(out, 0, sizeof(msg));

    if (strcmp(pa.users[0].passwd, req->adminKey) != 0)
    {
        trace(DEBUG_ERR, USER, "Passwd invalid !!!");
        status = CFGMGR_PASSWD_INVALID;
        goto doSuperUserMgrExit;
    }

    if (strlen(req->newKey) > USR_KEY_LNE_MAX)
    {
        trace(DEBUG_ERR, USER, "New Passwd len %d > %d !!!", strlen(req->newKey), USR_KEY_LNE_MAX);
        status = CFGMGR_PASSWD_INVALID;
        goto doSuperUserMgrExit;
    }

    if (strcmp(pa.users[0].userName, req->userName) == 0)
    {
        strncpy(pa.users[0].passwd, req->newKey, sizeof(pa.users[0].passwd));
    }
    else if (strcmp(pa.users[1].userName, req->userName) == 0)
    {
        strncpy(pa.users[1].passwd, req->newKey, sizeof(pa.users[1].passwd));
    }
    else
    {
        trace(DEBUG_ERR, USER, "User %s not exist !!!", req->userName);
        status = CFGMGR_USER_NOT_EXIST;
        goto doSuperUserMgrExit;
    }

    if(CFGMGR_OK != (status = paramSave(&pa)))
    {
        trace(DEBUG_ERR, USER, "paramSave failed.");
        goto doSuperUserMgrExit;
    }

    trace(DEBUG_INFO, USER, "Super User Manage succ");

doSuperUserMgrExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doSysTimeGet(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    sysTimeGetResponse *resp = (sysTimeGetResponse *)out->data;

//    trace(DEBUG_INFO, USER, "SysTimeGet start");

    resp->currentTime = time(NULL);
    out->type = MSGTYPE_SYSTIMEGET_RESPONSE;

//    trace(DEBUG_INFO, USER, "SysTimeGet succ");

    return status;
}

static cfgMgrStatus doSysTimeSet(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    sysTimeSetRequest *req = (sysTimeSetRequest *)in->data;

    trace(DEBUG_INFO, USER, "SysTimeSet start");

    memset(out, 0, sizeof(msg));

    stime(&req->correctTime);

    trace(DEBUG_INFO, USER, "SysTimeSet succ");

    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doAdminPasswdConfirm(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    adminPasswdConfirmRequest *req = (adminPasswdConfirmRequest *)in->data;

    if (strcmp(pa.users[0].passwd, req->adminPasswd) != 0)
    {
        status = CFGMGR_PASSWD_INVALID;
        trace(DEBUG_INFO, USER, "Passwd Invalid");
    }

    genConfirmMsg(status, out);

    return status;
}


static int logicVersionGet (void)
{
    int ver = 0x1000000;

    return ver;
}

static cfgMgrStatus doVersionGet(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    versionGetResponse *resp = (versionGetResponse *)out->data;
    int logicVersion;

    trace(DEBUG_INFO, USER, "VersionGet start");

    memset (out, 0, sizeof(msg));

    snprintf(resp->cfgMgrVersion, sizeof(resp->cfgMgrVersion), "V%d.%d", CFGMGR_MAJOR ,CFGMGR_MINOR);
    logicVersion = logicVersionGet();
    snprintf(resp->logicVersion, sizeof(resp->logicVersion), "V%d.%d", logicVersion>>24, logicVersion>>16 & 0xff);    
    
    out->type = MSGTYPE_GETVERSION_RESPONSE;
    
    trace(DEBUG_INFO, USER, "VersionGet succ");

    return status;
}

static cfgMgrStatus doFactoryReset(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "FactoryReset start");

    status  = CFGMGR_NOT_SUPPORT;
    trace(DEBUG_ERR, USER, "FactoryReset not support !!!");
    goto factoryResetExit;

    trace(DEBUG_INFO, USER, "FactoryReset succ");
    
factoryResetExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doReboot(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "Reboot start");

    status  = CFGMGR_NOT_SUPPORT;
    trace(DEBUG_ERR, USER, "Reboot not support !!!");
    goto rebootExit;

    trace(DEBUG_INFO, USER, "Reboot succ");
    
rebootExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doLogLookUp(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    logLookUpRequest *req = (logLookUpRequest *)in->data;
    logLookUpResponse * resp = (logLookUpResponse *)out->data;
    int recordsTotal, i;

//    trace(DEBUG_INFO, USER, "Log Look Up start");

    memset(out, 0, sizeof(msg));

    resp->recordsTotal = logRequest(req->startTime, req->endTime, req->logType,
        req->logSignificance, req->start, resp->elements, MIN(PAGE_RECORDS_MAX, req->length));
    resp->draw = req->draw;
    resp->length= MIN(req->length, MIN(resp->recordsTotal - req->start, PAGE_RECORDS_MAX));

    out->type = MSGTYPE_LOGLOOKUP_RESPONSE;

//    trace(DEBUG_INFO, USER, "Log Look Up succ");

    return status;
}

static cfgMgrStatus doLogExport(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    logExportRequest *req = (logExportRequest *)in->data;
    logExportResponse *resp = (logExportResponse *)out->data;    

    trace(DEBUG_INFO, USER, "Log Export start");

    memset(out, 0, sizeof(msg));

    resp->recordsTotal = logRequestExport(req->startTime, req->endTime, req->logType,req->logSignificance);
    strncpy(resp->logSearchResult, "tmp/logSearchResult.txt", sizeof(resp->logSearchResult));
    
    out->type = MSGTYPE_LOGEXPORT_RESPONSE;

    trace(DEBUG_INFO, USER, "Log Export succ");

    return status;
}

static cfgMgrStatus doLogClearAll(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    char cmd[100] = {0};

    trace(DEBUG_INFO, USER, "Log Clear ALL");

    snprintf(cmd, sizeof(cmd), "rm %s", LOG_DATA_BASE_FILE_NAME);

    system(cmd);
    
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doDiskInfo(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    diskInfoResponse * resp = (diskInfoResponse *)out->data;

    trace(DEBUG_INFO, USER, "Disk Info start");

    memset(out, 0, sizeof(msg));
#if 1
    strncpy(resp->modelNumber, "TOSHIBA DT01ACA100", DISK_INFO_STRING_LEN_MAX);
    strncpy(resp->sn, "48DY7MRMS", DISK_INFO_STRING_LEN_MAX);
    strncpy(resp->firwareRevision, "MS2OA810", DISK_INFO_STRING_LEN_MAX);
    strncpy(resp->cacheBufferSize, "23652 KBytes", DISK_INFO_STRING_LEN_MAX);
    strncpy(resp->rate, "7200", DISK_INFO_STRING_LEN_MAX);
    strncpy(resp->formFactor, "3.5 inch", DISK_INFO_STRING_LEN_MAX);
    strncpy(resp->temp, "36", DISK_INFO_STRING_LEN_MAX);
    strncpy(resp->size, "1000000000000", DISK_INFO_STRING_LEN_MAX);
    strncpy(resp->avail, "500000000000", DISK_INFO_STRING_LEN_MAX);
    strncpy(resp->used, "500000000000", DISK_INFO_STRING_LEN_MAX);
#else
    //TODO
#endif
    out->type = MSGTYPE_DISKINFO_RESPONSE;

    trace(DEBUG_INFO, USER, "Disk Info succ");

    return status;
}

static cfgMgrStatus doSystemInfo(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    systemInfoResponse * resp = (systemInfoResponse *)out->data;

    trace(DEBUG_INFO, USER, "System Info start");

    memset(out, 0, sizeof(msg));

#if 1
    strncpy(resp->hwVer, "V1.0", SYSTEM_INFO_STRING_LEN_MAX);
    strncpy(resp->sn, "xxxx-xxxx-xxxx", SYSTEM_INFO_STRING_LEN_MAX);
    strncpy(resp->cfgMgrVersion, "V1.0", SYSTEM_INFO_STRING_LEN_MAX);
    strncpy(resp->logicVersion, "V1.0", SYSTEM_INFO_STRING_LEN_MAX);
    resp->cfgMgrLastUpdateTime = time(NULL);
    resp->logicLastUpdateTime = time(NULL);
    resp->currentTime = time(NULL);
    resp->runningSec = 3600;
    resp->lan1Status.linkSpeed = 100000000;
    resp->lan1Status.linkStat = LINK_UP;
    resp->lan1Status.nRecvPackages = 100;
    resp->lan1Status.nSendPackages = 100;
    resp->lan2Status.linkSpeed = 100000000;
    resp->lan2Status.linkStat = LINK_UP;
    resp->lan2Status.nRecvPackages = 100;
    resp->lan2Status.nSendPackages = 100;
#else
    //TODO
#endif
    out->type = MSGTYPE_SYSTEMINFO_RESPONSE;

    trace(DEBUG_INFO, USER, "System Info succ");

    return status;
}

static cfgMgrStatus lanStatisticsClear(int netNumber)
{
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "lan%dStatisticsClear start", netNumber);

    status  = CFGMGR_NOT_SUPPORT;
    trace(DEBUG_ERR, SYSTEM, "lanStatisticsClear not support !!!");
    goto lanStatisticsClearExit;
    
lanStatisticsClearExit:    
    return status;
}

static cfgMgrStatus doLan1StatisticsClear(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "Lan1 Statistics Clear");

    status = lanStatisticsClear(1);

    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doLan2StatisticsClear(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "Lan2 Statistics Clear");

    status = lanStatisticsClear(2);

    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doUpdateLogicFile(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "UpdateLogicFile start");

    //TODO

    trace(DEBUG_INFO, USER, "UpdateLogicFile succ");

//doUpdateLogicFileExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doUpdateCfgMgrFile(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "UpdateCfgMgrFile start");

    //TODO

    trace(DEBUG_INFO, USER, "UpdateCfgMgrFile succ");

//doUpdateCfgMgrFileExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doUpdateWeb(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "UpdateWebstart");

    //TODO

    trace(DEBUG_INFO, USER, "UpdateWeb succ");

//doUpdateCfgMgrFileExit:
    genConfirmMsg(status, out);

    return status;
}









static void webProcess (void)
{
    int len, netNumber;
    msgID mId;
    msg recvMsg, sendMsg;
    cfgMgrStatus status = CFGMGR_OK;
    netParam *net;
    
    /** set process name */
    prctl(PR_SET_NAME, WEB_THREAD_NAME);

    /** open message */
//    mq_unlink(CGI_CFGMGR_MSG_NAME);
    if((msgID)-1 == (mId = msgOpen(CGI_CFGMGR_MSG_NAME)))
    {
        trace(DEBUG_ERR, SYSTEM, "msgOpen %s error !!!", CGI_CFGMGR_MSG_NAME);
        return;
    }    
    

    /** load parameters */
    if(CFGMGR_OK != (status = paramLoad(&pa)))
    {
        trace(DEBUG_ERR, SYSTEM, "paramLoad failed(%d)", (int)status);
        goto webProcessExit;
    }
    /** set net parameters */
    for(netNumber = 1; netNumber <= 2; netNumber++)
    {
        if (netNumber == 1)
            net = &pa.lan1.net;
        else
            net = &pa.lan2.net;
        if(CFGMGR_OK != (status = setNetParameters(net, netNumber)))
        {
            trace(DEBUG_ERR, SYSTEM, "webProcess setNetParameters net%d failed!!!", netNumber);
            goto webProcessExit;
        }
    }

    while(1)
    {
        if((len = msgRecv(mId, &recvMsg)) <= 0)
        {
            trace(DEBUG_ERR, SYSTEM, "msgRecv %s error !!!", CGI_CFGMGR_MSG_NAME);
            break;
        }

        trace(DEBUG_INFO, SYSTEM, "msgRecv a message, type %d.", recvMsg.type);

        switch(recvMsg.type)
        {
            case MSGTYPE_LOGIN_REQUEST:
                status = doLogin(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_LAN1TEST:
                status = doLanTest(&recvMsg, &sendMsg, 1);
                break;
            case MSGTYPE_LAN2TEST:
                status = doLanTest(&recvMsg, &sendMsg, 2);
                break;
            case MSGTYPE_NETCONFIGSAVE:                
                status = doNetConfigSave(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_NETCAPTURE_REQUEST:                
                status = doNetCapture(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_NETFILTER_REQUEST:
                status = doNetFilter(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_FILELOOKUP_REQUEST:
                status = doFileLookUp(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_FILEUPLOAD_REQUEST:
                status = doFileUpLoad(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_NORMALUSERMGR_REQUEST:
                status = doNormalUserMgr(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_SUPERUSERMGR_REQUEST:
                status = doSuperUserMgr(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_SYSTIMEGET_REQUEST:
                status = doSysTimeGet(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_SYSTIMESET_REQUEST:
                status = doSysTimeSet(&recvMsg, &sendMsg);
                break;            
            case MSGTYPE_ADMINPASSWDCOMFIRM_REQUEST:
                status = doAdminPasswdConfirm(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_GETVERSION_REQUEST:
                status = doVersionGet(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_FACTORYRESET_REQUEST:
                status = doFactoryReset(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_REBOOT_REQUEST:
                status = doReboot(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_LOGLOOKUP_REQUEST:
                status = doLogLookUp(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_LOGEXPORT_REQUEST:
                status = doLogExport(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_LOGCLEARALL_REQUEST:
                status = doLogClearAll(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_DISKINFO_REQUEST:
                status = doDiskInfo(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_SYSTEMINFO_REQUEST:
                status = doSystemInfo(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_LAN1STATISTICSCLEAR_REQUEST:
                status = doLan1StatisticsClear(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_LAN2STATISTICSCLEAR_REQUEST:
                status = doLan2StatisticsClear(&recvMsg, &sendMsg);
                break;            
            case MSGTYPE_UPDATELOGICFILE_REQUEST:
                status = doUpdateLogicFile(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_UPDATECFGMGRFILE_REQUEST:
                status = doUpdateCfgMgrFile(&recvMsg, &sendMsg);
                break;
            case MSGTYPE_UPDATEWEB_REQUEST:
                status = doUpdateWeb(&recvMsg, &sendMsg);
                break;
            
            default:
                trace(DEBUG_ERR, USER, "Operation not support!!!, msgtype[%d]", recvMsg.type);
                status = CFGMGR_NOT_SUPPORT;
                genConfirmMsg(status, &sendMsg);
                break;
        }

        if(-1 == msgSend(mId, &sendMsg))
        {
            trace(DEBUG_ERR, SYSTEM, "msgSend failed !!!");
            goto webProcessExit;
        }        
    }

webProcessExit:

    msgClose(mId);
}
 
int webInit (void)
{
	int ret;
	pthread_attr_t attr;

    mq_unlink(CFGMGR_DUMPCAP_MSG0_NAME);
    if((msgID)-1 == (cfgmgrDumpcapMsgId0 = msgOpen(CFGMGR_DUMPCAP_MSG0_NAME)))
    {
        trace(DEBUG_ERR, SYSTEM, "msgOpen %s error !!!", CFGMGR_DUMPCAP_MSG0_NAME);
        return -1;
    }
    mq_unlink(CFGMGR_DUMPCAP_MSG1_NAME);
    if((msgID)-1 == (cfgmgrDumpcapMsgId1 = msgOpen(CFGMGR_DUMPCAP_MSG1_NAME)))
    {
        trace(DEBUG_ERR, SYSTEM, "msgOpen %s error !!!", CFGMGR_DUMPCAP_MSG1_NAME);
        return -1;
    }
    
	
	ret = pthread_attr_init(&attr);
	assert(ret == 0);
	ret = pthread_attr_setstacksize(&attr, WEB_THREAD_STACK_SIZE);
	assert(ret == 0);
	
	ret = pthread_create(&webThreadId, &attr, (void *) webProcess, NULL);
	if(ret != 0)
		trace(DEBUG_ERR, SYSTEM, "Create pthread error[%d]!", ret);

    return 0;
}

