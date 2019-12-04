#include <stdio.h>
#include "../inc/cfgMgrMessage.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <trace.h>


#ifndef SYSTEM_V_MQUEUE
#define MSGQ_NAME    "/mymqtest"
#else
#define MSGQ_NAME    "0x90000000"
#endif

#define WEB_THREAD_NAME "CfgMgrWebThread"


int main (int argc, char ** argv)
{
    int i;
    int n;
    msgID recvMsgId, sendMsgId;
    msg m;
    int len;
    
    n = atoi(argv[1]);

#ifndef SYSTEM_V_MQUEUE
    mId = msgOpen(argv[2]);
#else
    key_t key;
    key = atoi(argv[2]);

    recvMsgId = msgOpen(CGI_2_CFGMGR_MSG_NAME);   
    sendMsgId = msgOpen(CFGMGR_2_CGI_MSG_NAME);   
#endif
    if ((-1 == recvMsgId) || (-1 == sendMsgId))
    {
        printf("open %s failed\n.", MSGQ_NAME);
        return -1;
    }
    if (1 == n)
    {
        while(1)
        {
            if ((len = msgRecv(recvMsgId, &m)) < 0 )
            {
                printf("11111111 : recv err\n");
                return -1;
            }
            else
            {
                printf("11111111 : recv %d bytes\n", len);
            }
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
            if (0 != msgSend(sendMsgId, &m))
                printf ("2222222222 : send failed\n");
    }
    
    return 0;    
}

