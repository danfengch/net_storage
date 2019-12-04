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
#include <stdio.h>
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
#include <cfgMgrMessage.h>
#include <assert.h>
#include <mxml.h>
#include <version.h>
#include <share.h>
#include <aid.h>
#include <sys/types.h>
#include <dirent.h>
#include <fpgaRemoteUpdate.h>


#define ETHER_ADDR_LEN    6
#define UP    1
#define DOWN    0
#define BCD2HEX(x) (((x) >> 4) * 10 + ((x) & 0x0F))       /*20H -> 20*/
#define HEX2BCD(x) (((x) % 10) + ((((x) / 10) % 10) << 4))  /*20 -> 20H*/
#define MAX(x, y)   (((x) < (y)) ? (y) : (x))
#define MIN(x, y)   (((x) < (y)) ? (x) : (y))





#define WEB_THREAD_NAME "CfgMgrWebThread"


pthread_t webThreadId = -1;

static param pa;
static msgID cfgmgr2CgiMsgId;
static msgID cgi2CfgmgrMsgId;
static msgID cfgmgr2DumpcapMsgId0;
static msgID cfgmgr2DumpcapMsgId1;
static msgID dumpcap2CfgmgrMsgId0;
static msgID dumpcap2CfgmgrMsgId1;


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
			p->lan1.capture.isAutoUpLoad = TRUE;
		else
			p->lan1.capture.isAutoUpLoad = FALSE;
        trace(DEBUG_INFO, SYSTEM, "Lan1_AutoUpLoadEnable %s", p->lan1.capture.isAutoUpLoad ? "true":"false");
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
			p->lan2.capture.isAutoUpLoad = TRUE;
		else
			p->lan2.capture.isAutoUpLoad = FALSE;
        trace(DEBUG_INFO, SYSTEM, "Lan2_AutoUpLoadEnable %s", p->lan2.capture.isAutoUpLoad ? "true":"false");
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

int get_mac_addr(char *ifname, unsigned char *mac)
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

typedef struct
{
    linkStatus lk;
    int        speed;
}linkInfo;

int ethGetLinkInfo(char *ifName, linkInfo * lkinfo)
{
    int skfd; 
    struct ifreq ifr; 
    struct ethtool_value edata; 
    struct ethtool_cmd ep;
    
    edata.cmd = ETHTOOL_GLINK; 
    edata.data = 0; 
    
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, ifName); 
    
    ifr.ifr_data = (char *) &edata; 
    
    if(( skfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0) 
    {
        return -1;
    }
    
    if(ioctl( skfd, SIOCETHTOOL, &ifr ) == -1) 
    {
        return -1;
    }
    else
    {
        lkinfo->lk = (edata.data > 0) ? LINK_UP: LINK_DOWN;
    }

    if(lkinfo->lk == LINK_DOWN)
    {
        lkinfo->speed = 0;
        goto ethGetLinkInfoExit;
    }
    
    ep.cmd = ETHTOOL_GSET;
    ifr.ifr_data = (caddr_t)&ep;

    if(ioctl( skfd, SIOCETHTOOL, &ifr ) == -1) 
    {
        close(skfd);
        return -1;
    }
    else
    {
        lkinfo->speed = ep.speed;
//        trace(DEBUG_INFO, USER, "link speed : %d", lkinfo->speed);
    }
    
ethGetLinkInfoExit:    
    close(skfd); 
    
    return 0;
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

#ifdef MAC_CONFIG
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
#endif

static cfgMgrStatus setNetParameters(netParam *net, int netNumber)
{
    cfgMgrStatus status = CFGMGR_OK;
    unsigned char buffer[100];
    
    /** set ip */
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
//    trace(DEBUG_ERR, SYSTEM, "current hw address : Mac %02x %02x %02x %02x %02x %02x.", 
//            buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
#ifdef MAC_CONFIG
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

    /** net parameter set bak */
    if (netNumber == 1)
        netOrigin = &pa.lan1.net;
    else
        netOrigin = &pa.lan2.net;

    /** set net parameters */
    if(CFGMGR_OK != (status = setNetParameters(net, netNumber)))
    {
        trace(DEBUG_ERR, USER, "Lan %d test : setNetParameters failed !!!", netNumber);
        goto doLanTestExit;
    }
     
    /** lan test */
    if (0 != pingTest(req->destIp))
    {
        trace(DEBUG_ERR, USER, "Network is unreachable");
        status = CFGMGR_NETWORK_UNREACHABLE;
        goto doLanTestExit;
    }

    trace(DEBUG_INFO, USER, "Lan %d test OK", netNumber);
    
doLanTestExit:

    setNetParameters(netOrigin, netNumber);
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
    msg             sendMsg, recvMsg;
    cfgMgrStatus    status = CFGMGR_ERR;
    int             len;
    int             *isCapture;
    msgID           sendMsgId, recvMsgId;

    sendMsg.type = MSGTYPE_NETCAPTURE_REQUEST;
    isCapture = (int *)sendMsg.data;
    *isCapture = capture->isCapture;

    if (netNumber == 1)
    {
        sendMsgId = cfgmgr2DumpcapMsgId0;
        recvMsgId = dumpcap2CfgmgrMsgId0;
    }
    else
    {
        sendMsgId = cfgmgr2DumpcapMsgId1;
        recvMsgId = dumpcap2CfgmgrMsgId1;
    }

//    sendMsgId = cfgmgr2DumpcapMsgId0;
//    recvMsgId = dumpcap2CfgmgrMsgId0;
#ifdef NET_CAPTURE
    if(-1 == msgSend(sendMsgId, &sendMsg))
    {
        trace(DEBUG_ERR, SYSTEM, "%s : msgSend failed !!!", __FUNCTION__);
        goto netCaptureExit;
    }
    trace(DEBUG_INFO, USER, "%s : send msg to dumpcap", __FUNCTION__);
    if((len = msgRecv(recvMsgId, &recvMsg)) <= 0)
    {
        trace(DEBUG_ERR, SYSTEM, "%s : net%d msgRecv from dumpcap error !!!", 
            __FUNCTION__, netNumber);
        goto netCaptureExit;
    }
#endif
    status = CFGMGR_OK;

netCaptureExit:

    return status;
}

static cfgMgrStatus doNetCapture(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    captureParam *capture = (captureParam *)in->data;
    int isChanged = 0;

    trace(DEBUG_INFO, USER, "Net Capture start");

    if (capture->isCapture != pa.lan1.capture.isCapture)
    {
        if(CFGMGR_OK != (status = netCapture(capture, 1)))
        {
            trace(DEBUG_ERR, USER, "net 1 netCapture failed.");
            goto doNetCaptureExit;
        }
        memcpy(&pa.lan1.capture, capture, sizeof(captureParam));
        isChanged = TRUE;
    }
        
    capture++;
    if (capture->isCapture != pa.lan2.capture.isCapture)
    {
        if(CFGMGR_OK != (status = netCapture(capture, 2)))
        {
            trace(DEBUG_ERR, USER, "net 2 netCapture failed.");
            goto doNetCaptureExit;
        }
        memcpy(&pa.lan2.capture, capture, sizeof(captureParam));
        isChanged = TRUE;
    }

    if (isChanged)
    {
        if (CFGMGR_OK != (status = paramSave(&pa)))
        {
            trace(DEBUG_ERR, USER, "paramSave failed.");
            goto doNetCaptureExit;
        }
    }

    trace(DEBUG_INFO, USER, "Net Capture succ");

doNetCaptureExit:
    genConfirmMsg(status, out);

    return status;
}

/*
1.功能编码
    Gpio18	Gpio0	Gpio1	Gpio2	说明
    0	0	0	0	通道0 不更新（给陈伦）
        0	0	1	通道0 10M
        0	1	0	通道0 更新（给陈伦）
        0	1	1	通道0 100M
        1	0	0	无
        1	0	1	通道0 1000M
        1	1	1	打开过滤
        1	1	0	关闭过滤
    1	0	0	0	通道1 不更新（给陈伦）
        0	0	1	通道1  10M
        0	1	0	通道1 更新（给陈伦）
        0	1	1	通道1  100M
        1	0	0	无
        1	0	1	通道1  1000M
        1	1	1	打开过滤
        1	1	0	关闭过滤
2.使能管脚
    gpio19 作为使能信号使用，高电平有效
*/
static const int opCode[][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {0, 1, 1, 1},
    {0, 1, 1, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 1},
    {1, 0, 1, 0},
    {1, 0, 1, 1},
    {1, 1, 0, 0},
    {1, 1, 0, 1},
    {1, 1, 1, 1},
    {1, 1, 1, 0},
};

typedef enum
{
    OPCODE_NET0_FPGA_UPDATE_OFF = 0,
    OPCODE_NET0_SPEED_10M,
    OPCODE_NET0_FPGA_UPDATE_ON,
    OPCODE_NET0_SPEED_100M,
    OPCODE_NET0_NONE,
    OPCODE_NET0_SPEED_1000M,
    OPCODE_NET0_FILTER_ON,
    OPCODE_NET0_FILTER_OFF,
    OPCODE_NET1_FPGA_UPDATE_OFF,
    OPCODE_NET1_SPEED_10M,
    OPCODE_NET1_FPGA_UPDATE_ON,
    OPCODE_NET1_SPEED_100M,
    OPCODE_NET1_NONE,
    OPCODE_NET1_SPEED_1000M,
    OPCODE_NET1_FILTER_ON,
    OPCODE_NET1_FILTER_OFF
}fpgaOperateCode;

static void fgpaCtrlInit(void)
{
    DIR *dir;

    if (NULL == (dir = opendir("/sys/class/gpio/gpio19")))
    {
        system("echo 19 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio19/direction");    
    }
    else
    {
        closedir(dir);
    }
    system("echo 0 > /sys/class/gpio/gpio19/value");

    if (NULL == (dir = opendir("/sys/class/gpio/gpio0")))
    {
        system("echo 0 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio0/direction");
    }
    else
    {
        closedir(dir);
    }

    if (NULL == (dir = opendir("/sys/class/gpio/gpio1")))
    {
        system("echo 1 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio1/direction");
    }
    else
    {
        closedir(dir);
    }

    if (NULL == (dir = opendir("/sys/class/gpio/gpio2")))
    {
        system("echo 2 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio2/direction");
    }
    else
    {
        closedir(dir);
    }

    if (NULL == (dir = opendir("/sys/class/gpio/gpio18")))
    {
        system("echo 18 > /sys/class/gpio/export");
        system("echo \"out\" > /sys/class/gpio/gpio18/direction");    
    }
    else
    {
        closedir(dir);
    }
}

static void fpgaCtrl (fpgaOperateCode code)
{
    char cmd[50];
    
    system("echo 0 > /sys/class/gpio/gpio19/value");
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio18/value", opCode[code][0]);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio0/value", opCode[code][1]);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio1/value", opCode[code][2]);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio2/value", opCode[code][3]);
    system(cmd);
    system("echo 1 > /sys/class/gpio/gpio19/value");
    usleep(1000);
    system("echo 0 > /sys/class/gpio/gpio19/value");
}

static cfgMgrStatus netFilter(filterParam *filter, int netNumber)
{
    cfgMgrStatus     status = CFGMGR_ERR;    
    fpgaOperateCode  code;

    if ((netNumber == 1) && (filter->isFilter == FALSE))
        code = OPCODE_NET0_FILTER_OFF;
    else if ((netNumber == 1) && (filter->isFilter == TRUE))
        code  = OPCODE_NET0_FILTER_ON;
    else if ((netNumber == 2) && (filter->isFilter == FALSE))
        code  = OPCODE_NET1_FILTER_OFF;
    else if ((netNumber == 2) && (filter->isFilter == TRUE))
        code  = OPCODE_NET1_FILTER_ON;
    else
        goto netFilterExit;
    
    fpgaCtrl (code);

    status = CFGMGR_OK;

netFilterExit:
    return status;
}

static cfgMgrStatus doNetFilter(msg *in, msg *out)
{
    cfgMgrStatus    status = CFGMGR_OK;
    filterParam     *filter = (filterParam *)in->data;
    int             isChanged = FALSE;

    trace(DEBUG_INFO, USER, "Net Filter start");

    if(filter->isFilter != pa.lan1.filter.isFilter)
    {
        if(CFGMGR_OK != (status = netFilter(filter, 1)))
        {
            trace(DEBUG_ERR, SYSTEM, "net 1 netFilter failed.");
            goto doNetFilterExit;
        }
        memcpy(&pa.lan1.filter, filter, sizeof(filterParam));
        isChanged = TRUE;
    }
    
    filter++;

    if(filter->isFilter != pa.lan2.filter.isFilter)
    {
        if(CFGMGR_OK != (status = netFilter(filter, 2)))
        {
            trace(DEBUG_ERR, SYSTEM, "net 2 netFilter failed.");
            goto doNetFilterExit;
        } 
        memcpy(&pa.lan2.filter, filter, sizeof(filterParam));
        isChanged = TRUE;
    }

    if(isChanged)
    {
        if(CFGMGR_OK != (status = paramSave(&pa)))
        {
            trace(DEBUG_ERR, USER, "paramSave failed.");
            goto doNetFilterExit;
        }
    }

    trace(DEBUG_INFO, USER, "Net Filter succ");

doNetFilterExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doFileLookUp(msg *in, msg *out)
{
    cfgMgrStatus       status = CFGMGR_OK;
    fileLookUpRequest  *req = (fileLookUpRequest *)in->data;
    int                len;
    msgID              sendMsgId, recvMsgId;
    msg                *sendMsg = in, *recvMsg = out;

    trace(DEBUG_INFO, USER, "File Look Up start");

    memset(out, 0, sizeof(msg));

    /** file look up */
    if (req->netNumber == 1)
    {
        sendMsgId = cfgmgr2DumpcapMsgId0;
        recvMsgId = dumpcap2CfgmgrMsgId0;
    }
    else
    {
        sendMsgId = cfgmgr2DumpcapMsgId1;
        recvMsgId = dumpcap2CfgmgrMsgId1;
    }

//    sendMsgId = cfgmgr2DumpcapMsgId0;
//    recvMsgId = dumpcap2CfgmgrMsgId0;

    if(-1 == msgSend(sendMsgId, sendMsg))
    {
        trace(DEBUG_ERR, SYSTEM, "%s : net%d msgSend failed !!!", __FUNCTION__, req->netNumber);
        goto doNetFilterExit;
    }
    trace(DEBUG_INFO, USER, "send msg to dumpcap");
    if((len = msgRecv(recvMsgId, recvMsg)) <= 0)
    {
        trace(DEBUG_ERR, SYSTEM, "%s : net%d msgRecv failed !!!", __FUNCTION__, req->netNumber);
        goto doNetFilterExit;
    }
//    trace(DEBUG_INFO, USER, "recv a msg from dumpcap ,msgtype %d", recvMsg->type);
//    trace(DEBUG_INFO, USER, "draw         : %d", resp->draw);
//    trace(DEBUG_INFO, USER, "recordsTotal : %d", resp->recordsTotal);
//    trace(DEBUG_INFO, USER, "length       : %d", resp->length);

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

    /* save file */
    system("sync");

    /* tell aid stop feed watchdog */
    rebootFlag = 1;

    trace(DEBUG_INFO, USER, "Reboot succ");
    
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doLogLookUp(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    logLookUpRequest *req = (logLookUpRequest *)in->data;
    logLookUpResponse * resp = (logLookUpResponse *)out->data;

//    trace(DEBUG_INFO, USER, "Log Look Up start");

    memset(out, 0, sizeof(msg));

    resp->recordsTotal = logRequest(req->startTime, req->endTime, req->lgType,
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

    resp->recordsTotal = logRequestExport(req->startTime, req->endTime, req->lgType,req->logSignificance);
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

static int getSmartctlAttrValue(FILE *fp, const char *name, char *val, int size)
{
    char line[1024];
    char *pos;
    int  len, copyLen = 0;
    
//    if (0 != fseek (fp, 0, SEEK_SET))
//    {
//        trace (DEBUG_ERR, SYSTEM, "%s : fseek error", __func__);
//        return -1;
//    }

    while (NULL != fgets (line, sizeof(line), fp))
    {
        if (NULL == (pos = strstr(line, name)))
            continue;

        len = strlen(name);
        //take out of ':'        
//        if (pos[len++] != ':')  continue;
        //take out of space
        while(pos[len] == ' ')  len++;

        //copy
        while((pos[len] != '\r') && (pos[len] != '\n') && (copyLen < (size - 1)))
        {
            val[copyLen++] = pos[len++];
        }
        val[copyLen] = 0;

        return 0;
    }

    val[copyLen++] = '-';
    val[copyLen++] = '-';
    val[copyLen++] = 0;
    
    return -1;
}

static int getDiskUseInfo (char *psize, char *pavail, char *pused, int lenMax)
{
    FILE *fp;
    char line[1024];
    unsigned long long size = 0, avail = 0, used = 0;
    char *pos;
    
    if (NULL == (fp = popen ("df -k  |grep /dev/sda", "r")))
    {
        trace (DEBUG_ERR, SYSTEM, "popen df -k  |grep /dev/sda error");
        goto getDiskUseInfoExit;
    }

    while(NULL != fgets(line, sizeof(line), fp))
    {
        if (NULL == (pos = strstr(line, "/dev/sda2")))
            continue;
        trace (DEBUG_INFO, SYSTEM, "%s", pos);

        pos += strlen("/dev/sda2");
        //take out of space
        while(*pos == ' ') pos++;
        trace (DEBUG_INFO, SYSTEM, "size : %s", pos);
        size = (unsigned long long)1024 * atoi(pos);
        //take out of szie & space
        while(*pos != ' ') pos++;
        while(*pos == ' ') pos++;
        trace (DEBUG_INFO, SYSTEM, "used : %s", pos);
        used = (unsigned long long)1024 * atoi(pos);
        //take out of used & space
        while(*pos != ' ') pos++;
        while(*pos == ' ') pos++;
        trace (DEBUG_INFO, SYSTEM, "avail : %s", pos);
        avail = (unsigned long long)1024 * atoi(pos);

        break;
    }

    pclose (fp);

getDiskUseInfoExit:
    snprintf (psize, lenMax, "%lld", size);
    snprintf (pavail, lenMax, "%lld", avail);
    snprintf (pused, lenMax, "%lld", used);

    return 0;
}

static cfgMgrStatus doDiskInfo(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;
    diskInfoResponse * resp = (diskInfoResponse *)out->data;
    FILE *fp;

    trace(DEBUG_INFO, USER, "Disk Info start");

    memset(out, 0, sizeof(msg));

    if (NULL == (fp = popen ("smartctl -a /dev/sda", "r")))
    {
        trace (DEBUG_ERR, SYSTEM, "popen smartctl -a /dev/sda error");
        return CFGMGR_ERR;
    }

    getSmartctlAttrValue(fp, "Device Model:", resp->modelNumber, DISK_INFO_STRING_LEN_MAX);
    getSmartctlAttrValue(fp, "Serial Number:", resp->sn, DISK_INFO_STRING_LEN_MAX);
    getSmartctlAttrValue(fp, "Firmware Version:", resp->firwareRevision, DISK_INFO_STRING_LEN_MAX);    
    getSmartctlAttrValue(fp, "194 Temperature_Celsius     0x0000   100   100   000    Old_age   Offline      -", resp->temp, DISK_INFO_STRING_LEN_MAX);
    getSmartctlAttrValue(fp, "cacheBufferSize:", resp->cacheBufferSize, DISK_INFO_STRING_LEN_MAX);
    getSmartctlAttrValue(fp, "rate:", resp->rate, DISK_INFO_STRING_LEN_MAX);
    getSmartctlAttrValue(fp, "formFactor:", resp->formFactor, DISK_INFO_STRING_LEN_MAX);
    pclose (fp);
    
    getDiskUseInfo (resp->size, resp->avail, resp->used, DISK_INFO_STRING_LEN_MAX);

    out->type = MSGTYPE_DISKINFO_RESPONSE;

    trace(DEBUG_INFO, USER, "Disk Info succ");

    return status;
}

static cfgMgrStatus captureGetStatistics(int netNumber, unsigned int *captureStatistics)
{
    cfgMgrStatus status = CFGMGR_NOT_SUPPORT;
    msg          sendMsg, recvMsg;
    msgID        sendMsgId, recvMsgId;
    int          len;

#ifdef CAPTURE_STATISTICS
    if (netNumber == 1)
    {
        sendMsgId = cfgmgr2DumpcapMsgId0;
        recvMsgId = dumpcap2CfgmgrMsgId0;
    }
    else if (netNumber == 2)
    {
        sendMsgId = cfgmgr2DumpcapMsgId1;
        recvMsgId = dumpcap2CfgmgrMsgId1;
    }
    else
    {
        status = CFGMGR_NET_NUMBER_INVALID;
        goto getCaptureStatisticsExit;
    }

    sendMsg.type = MSGTYPE_CAPTURE_STATISTICS_REQUEST;

    if(-1 == msgSend(sendMsgId, &sendMsg))
    {
        trace(DEBUG_ERR, SYSTEM, "%s : msgSend failed !!!", __FUNCTION__);
        goto getCaptureStatisticsExit;
    }
    trace(DEBUG_INFO, SYSTEM, "%s : lan%d send capture statistics request msg to dumpcap", __FUNCTION__, netNumber);
    if((len = msgRecv(recvMsgId, &recvMsg)) <= 0)
    {
        trace(DEBUG_ERR, SYSTEM, "%s : net%d msgRecv from dumpcap error !!!", 
            __FUNCTION__, netNumber);
        goto getCaptureStatisticsExit;
    }
    trace(DEBUG_INFO, SYSTEM, "%s : lan%d recv capture statistics request msg from dumpcap", __FUNCTION__, netNumber);
    *captureStatistics = *(unsigned int *)recvMsg.data;

    status = CFGMGR_OK;
#endif

getCaptureStatisticsExit:

    return status;
}

static int getRuntime (void)
{
    int fd;
    char buffer[50];
    int cnt = 0;
    int runtime = -1;
    
    if (0 > (fd = open("/proc/uptime", O_RDONLY)))
    {
        trace(DEBUG_ERR, SYSTEM,"open  /proc/uptime failed.\n");
        return runtime;
    }
    do
    {
        if (1 != read(fd, buffer + cnt, 1))
        {
            trace(DEBUG_ERR, SYSTEM,"read /proc/uptime failed.\n");
            goto exit;
        }
        cnt++;
    }while((buffer[cnt - 1] != ' ') && (buffer[cnt - 1] != EOF) && (cnt < sizeof(buffer)));
    
    if ((cnt < sizeof(buffer)) && (buffer[cnt - 1] == ' '))
    {
        buffer[cnt - 1] = 0;
        
        runtime = atoi(buffer);
    }
    else
    {
        trace(DEBUG_ERR, SYSTEM,"failed to read runtime from /proc/uptime\n");
    }
exit:    
    close(fd);
    return runtime;
}

static cfgMgrStatus doSystemInfo(msg *in, msg *out)
{
    cfgMgrStatus       status = CFGMGR_OK;
    systemInfoResponse *resp = (systemInfoResponse *)out->data;
    linkInfo           lkInfo;
    unsigned int       captureStatistics;

    trace(DEBUG_INFO, USER, "System Info start");

    memset(out, 0, sizeof(msg));

    strncpy(resp->hwVer, "V1.0", SYSTEM_INFO_STRING_LEN_MAX);
    strncpy(resp->sn, "xxxx-xxxx-xxxx", SYSTEM_INFO_STRING_LEN_MAX);
    strncpy(resp->cfgMgrVersion, "V1.0", SYSTEM_INFO_STRING_LEN_MAX);
    strncpy(resp->logicVersion, "V1.0", SYSTEM_INFO_STRING_LEN_MAX);
    resp->cfgMgrLastUpdateTime = time(NULL);
    resp->logicLastUpdateTime = time(NULL);
    resp->currentTime = time(NULL);
    resp->runningSec = getRuntime();
    if(0 != ethGetLinkInfo(NET1_NAME, &lkInfo))
    {
        trace(DEBUG_ERR, SYSTEM, "ethGetLinkInfo %s error.", NET1_NAME);
        lkInfo.lk = LINK_DOWN;
        lkInfo.speed = 0;
    }
    resp->lan1Status.linkSpeed = lkInfo.speed;
    resp->lan1Status.linkStat = lkInfo.lk;
    if (CFGMGR_OK != captureGetStatistics(1, &captureStatistics))
    {
        trace(DEBUG_ERR, SYSTEM, "%s : get lan1 captureStatistics error", __FUNCTION__);
        captureStatistics = 0;
    }
    resp->lan1Status.nRecvPackages = captureStatistics;
    resp->lan1Status.nSendPackages = 100;
    if(0 != ethGetLinkInfo(NET2_NAME, &lkInfo))
    {
        trace(DEBUG_ERR, SYSTEM, "ethGetLinkInfo %s error.", NET2_NAME);
        lkInfo.lk = LINK_DOWN;
        lkInfo.speed = 0;
    }
    resp->lan2Status.linkSpeed = lkInfo.speed;
    resp->lan2Status.linkStat = lkInfo.lk;
    if (CFGMGR_OK != captureGetStatistics(2, &captureStatistics))
    {
        trace(DEBUG_ERR, SYSTEM, "%s : get lan2 captureStatistics error", __FUNCTION__);
        captureStatistics = 0;
    }
    resp->lan2Status.nRecvPackages = captureStatistics;
    resp->lan2Status.nSendPackages = 100;

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

    system("arp -i eth0 -s 192.168.0.2 00:0a:35:01:fe:c0");

    fpgaCtrl (OPCODE_NET0_FPGA_UPDATE_ON);

    if (0 != fpgaRmtUdt(FPGA_UPDATE_FILE_NAME))
    {
        trace(DEBUG_INFO, USER, "Update logic config file failed");
        status = CFGMGR_ERR;
        goto doUpdateLogicFileExit;
    }

    fpgaCtrl (OPCODE_NET0_FPGA_UPDATE_OFF);

    system("sync");

    trace(DEBUG_INFO, USER, "UpdateLogicFile succ");

doUpdateLogicFileExit:
    genConfirmMsg(status, out);

    return status;
}

static cfgMgrStatus doUpdateCfgMgrFile(msg *in, msg *out)
{
    cfgMgrStatus status = CFGMGR_OK;

    trace(DEBUG_INFO, USER, "UpdateCfgMgrFile start");

    system("sync");

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









static int webProcess (void)
{
    int len, netNumber;
    msg recvMsg, sendMsg;
    cfgMgrStatus status = CFGMGR_OK;
    netParam *net;
    
    /** set process name */
    prctl(PR_SET_NAME, WEB_THREAD_NAME); 
    
#ifndef SYSTEM_V_MQUEUE
    mq_unlink(CGI_2_CFGMGR_MSG_NAME);
    mq_unlink(CFGMGR_2_CGI_MSG_NAME);
    mq_unlink(CFGMGR_2_DUMPCAP_MSG0_NAME);
    mq_unlink(CFGMGR_2_DUMPCAP_MSG1_NAME);
    mq_unlink(DUMPCAP_2_CFGMGR_MSG0_NAME);
    mq_unlink(DUMPCAP_2_CFGMGR_MSG1_NAME);
#endif
    if((msgID)-1 == (cgi2CfgmgrMsgId = msgOpen(CGI_2_CFGMGR_MSG_NAME)))
    {
        trace(DEBUG_ERR, SYSTEM, "msgOpen %s error !!!", CGI_2_CFGMGR_MSG_NAME);
        return -1;
    }

    if((msgID)-1 == (cfgmgr2CgiMsgId = msgOpen(CFGMGR_2_CGI_MSG_NAME)))
    {
        trace(DEBUG_ERR, SYSTEM, "msgOpen %s error !!!", CFGMGR_2_CGI_MSG_NAME);
        return -1;
    }

    if((msgID)-1 == (cfgmgr2DumpcapMsgId0 = msgOpen(CFGMGR_2_DUMPCAP_MSG0_NAME)))
    {
        trace(DEBUG_ERR, SYSTEM, "msgOpen %s error !!!", CFGMGR_2_DUMPCAP_MSG0_NAME);
        return -1;
    }

    if((msgID)-1 == (cfgmgr2DumpcapMsgId1 = msgOpen(CFGMGR_2_DUMPCAP_MSG1_NAME)))
    {
        trace(DEBUG_ERR, SYSTEM, "msgOpen %s error !!!", CFGMGR_2_DUMPCAP_MSG1_NAME);
        return -1;
    }

    if((msgID)-1 == (dumpcap2CfgmgrMsgId0 = msgOpen(DUMPCAP_2_CFGMGR_MSG0_NAME)))
    {
        trace(DEBUG_ERR, SYSTEM, "msgOpen %s error !!!", DUMPCAP_2_CFGMGR_MSG0_NAME);
        return -1;
    }

    if((msgID)-1 == (dumpcap2CfgmgrMsgId1 = msgOpen(DUMPCAP_2_CFGMGR_MSG1_NAME)))
    {
        trace(DEBUG_ERR, SYSTEM, "msgOpen %s error !!!", DUMPCAP_2_CFGMGR_MSG1_NAME);
        return -1;
    }

    /** load parameters */
    if(CFGMGR_OK != (status = paramLoad(&pa)))
    {
        trace(DEBUG_ERR, SYSTEM, "paramLoad failed(%d)", (int)status);
        goto webProcessExit;
    }

    /** set capture */
    if (CFGMGR_OK != netCapture(&pa.lan1.capture, 1))
    {
        trace(DEBUG_ERR, SYSTEM, "netCapture 1 excute failed !!!");
    }
    if (CFGMGR_OK != netCapture(&pa.lan2.capture, 2))
    {
        trace(DEBUG_ERR, SYSTEM, "netCapture 2 excute failed !!!");
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
    /** net filter */
    fgpaCtrlInit();
    if (CFGMGR_OK != netFilter(&pa.lan1.filter, 1))
    {
        trace(DEBUG_ERR, SYSTEM, "netFilter 1 excute failed !!!");
        goto webProcessExit;
    }
    if (CFGMGR_OK != netFilter(&pa.lan2.filter, 2))
    {
        trace(DEBUG_ERR, SYSTEM, "netFilter 2 excute failed !!!");
        goto webProcessExit;
    }
    
    /** get mac address */
    get_mac_addr(NET1_NAME, pa.lan1.net.mac);
    get_mac_addr(NET2_NAME, pa.lan2.net.mac);
    if(CFGMGR_OK != (status = paramSave(&pa)))
    {
        trace(DEBUG_ERR, USER, "paramSave failed.");
        goto webProcessExit;
    }

    while(1)
    {
        if((len = msgRecv(cgi2CfgmgrMsgId, &recvMsg)) <= 0)
        {
            trace(DEBUG_ERR, SYSTEM, "msgRecv %s error !!!", CGI_2_CFGMGR_MSG_NAME);
            break;
        }
#ifdef DEBUG_CFGMGR
        trace(DEBUG_INFO, SYSTEM, "msgRecv a message, type %d.", recvMsg.type);
#endif
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

        if(-1 == msgSend(cfgmgr2CgiMsgId, &sendMsg))
        {
            trace(DEBUG_ERR, SYSTEM, "msgSend failed !!!");
            goto webProcessExit;
        }        
    }

webProcessExit:
    if (status != CFGMGR_OK)
    {
        rebootFlag = 1;
    }
    
    return status;
}

int webInit (void)
{
	int ret;
	pthread_attr_t attr;

    system("dumpcap -i eth0 -w /usr/httproot/NetFiles/eth0 -b filesize:100000 -b files:2100 -t -P -B 256 &");
    system("dumpcap -i eth1 -w /usr/httproot/NetFiles/eth1 -b filesize:100000 -b files:2100 -t -P -B 256 &");
	
	ret = pthread_attr_init(&attr);
	assert(ret == 0);
	ret = pthread_attr_setstacksize(&attr, WEB_THREAD_STACK_SIZE);
	assert(ret == 0);
	
	ret = pthread_create(&webThreadId, &attr, (void *) webProcess, NULL);
	if(ret != 0)
		trace(DEBUG_ERR, SYSTEM, "Create pthread error[%d]!", ret);

    return 0;
}

