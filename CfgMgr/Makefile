include ./config.mk

OBJS = cfgMgr.o message.o trace.o web.o ping.o share.o

CfgMgr : $(OBJS)
	$(CC)  $(OBJS) -o $@  $(LIBS)
	cp message.o ./lib/mips64el/
	cp CfgMgr /home/chenxu/LOONGSON-2k1000/src/web/boa/
	cp CfgMgr ../webDebugWithZhang/
	cp CfgMgr /home/chenxu/mount/tftproot/
	rm -vf *.o
    
.c.o:
	echo Compiling $<
	$(CC) $(CFLAGS) $(INC) -c $<
    
clean:
	rm -vf *.o

