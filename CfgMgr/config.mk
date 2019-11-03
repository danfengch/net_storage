SHELL			=	/bin/sh

ARCH            =    mips64el
ifeq ($(ARCH), mips64el)
TOOLSCHAIN_PATH =   /home/chenxu/LOONGSON-2k1000/toolchain/gcc-4.9.3-64-gnu/bin/
TOOLSCHAIN_NAME =    mips64el-linux-
else
#endif
#ifeq ($(ARCH), x86)
TOOLSCHAIN_PATH =   
TOOLSCHAIN_NAME =   
endif
CFGMGR_PATH     =   /home/chenxu/LOONGSON-2k1000/src/CfgMgr
CC				=	$(TOOLSCHAIN_PATH)$(TOOLSCHAIN_NAME)gcc
AR				=	$(TOOLSCHAIN_PATH)$(TOOLSCHAIN_NAME)ar
RANLIB          =    $(TOOLSCHAIN_PATH)$(TOOLSCHAIN_NAME)ranlib
INC 			= 	-I$(CFGMGR_PATH) -I$(CFGMGR_PATH)/inc -I$(CFGMGR_PATH)/cgi-src
OPTIM	        =	-fPIC -O0 -g
#CFLAGS 			= 	$(OPTIM) $(INC) -Wall -std=c99 -D_THREAD_SAFE -D_REENTRANT
CFLAGS 			= 	$(OPTIM) $(INC) -Wall -D_THREAD_SAFE -D_REENTRANT 
LIBS			=	-lpthread -lrt -ldl -lsqlite3 $(CFGMGR_PATH)/lib/$(ARCH)/libmxml.a -L$(CFGMGR_PATH)/lib/$(ARCH)/
LDFLAGS 		= 	$(OPTIM) -Wl,-rpath

