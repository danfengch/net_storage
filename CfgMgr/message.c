/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     message.c
  * @author   Chenxu
  * @version  V1.0.0
  * @date     2019/09/25
  * @defgroup cfgMgr
  * @ingroup  cfgMgr
  * @brief    message function implement
  * @par History
  * Date          Owner         BugID/CRID        Contents
  * 2019/09/25    Chenxu        None              File Create
  ****************************************************************************** 
  */

#include "./inc/config.h"
#include "./inc/cfgMgrMessage.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>



#ifndef SYSTEM_V_MQUEUE
msgID msgOpen (char *msgName)
{
    msgID mId;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_msgsize = sizeof(msg);
    attr.mq_maxmsg = 10;
    attr.mq_curmsgs = 0;

    mId = (msgID)mq_open(msgName,IPC_CREAT|O_RDWR|O_CREAT, 
        S_IRUSR| S_IWUSR | S_IROTH| S_IWOTH|S_IRGRP|S_IWGRP, &attr);
    if ((msgID)-1 !=  mId)
    {

//        if(mq_getattr(mId, &attr) == -1)
//        {
//            printf("mq_getattr err\n");
//            mq_close(mId);
//            return (msgID) -1;
//        }
//    	//此处可以修改msgq_attr的相关属性   
//    	attr.mq_msgsize = sizeof(msg);
//        attr.mq_flags &= (~O_NONBLOCK);    	        	
//    	
//    	if(mq_setattr(mId, &attr, NULL) == -1)
//    	{
//            printf("mq_setattr err\n");
//            mq_close(mId);
//            return (msgID) -1;
//    	}

        if(mq_getattr(mId, &attr) == -1)
        {
            printf("mq_getattr err\n");
            mq_close(mId);
            return (msgID) -1;
        }

        printf("Queue \"%s\":\n\t- stores at most %ld messages\n\t- \
            large at most %ld bytes each\n\t- currently holds %ld messages\n\t- mq_flags[%08x]\n", 
            msgName, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs,attr.mq_flags);
    }

    return mId;
}


void msgClose (msgID id)
{
    mq_close((mqd_t)id);
}

int msgSend (msgID id, msg *m)
{
    if(mq_send((mqd_t)id, (char*)m, sizeof(msg), 1) == -1)
//    if(mq_send((mqd_t)id, (char*)m, 8192, 1) == -1)
    {
        return -1;
    }
    
	return 0;
}

int msgRecv (msgID id, msg *m)
{
    int len;
    
//    if ((len = (int)mq_receive((mqd_t)id, (char*)m, sizeof(msg), NULL)) == -1) 
    if ((len = (int)mq_receive((mqd_t)id, (char*)m, 8192, NULL)) == -1)
    {
        return -1;
    }

	return len;
}

#else
msgID msgOpen (key_t key)
{
    int    mId;
//    struct msqid_ds buf;

    mId = msgget((key_t)key, 0666 | IPC_CREAT);

//    if (-1 != mId)
//    {
//        if(0 != msgctl(mId, IPC_STAT, &buf))
//        {
//            printf("IPC_STAT failed.\n");
//            msgctl(mId, IPC_RMID, 0);
//            return (msgID)-1;
//        }

//        buf.
//        
//     }

    return (msgID)mId;
}

void msgClose (msgID id)
{
    return;
}

int msgSend (msgID id, msg *m)
{
    sysVMsg VMsg;

    VMsg.mtype = 1;

    memcpy(&VMsg.m, m, sizeof(msg));
    
    if(msgsnd((int)id, (const void *)&VMsg, sizeof(msg), 0) != 0)
    {
        return -1;
    }
    
	return 0;
}

int msgRecv (msgID id, msg *m)
{
    int len;
    sysVMsg VMsg;
    long int type = 1;
    
//    if ((len = (int)mq_receive((mqd_t)id, (char*)m, sizeof(msg), NULL)) == -1) 
//    if ((len = (int)mq_receive((mqd_t)id, (char*)m, 8192, NULL)) == -1)
    if ((len = msgrcv((int)id, (void *)&VMsg, sizeof(msg), type, MSG_NOERROR)) == -1)
    {
        return -1;
    }

    memcpy(m, &VMsg.m, sizeof(msg));

	return len;
}
#endif
