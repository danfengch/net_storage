include ./config.mk

CFGMGR_OBJS = web.o cfgMgr.o message.o trace.o ping.o share.o aid.o fpgaRemoteUpdate.o
MQTEST_OBJS = message.o trace.o ping.o share.o aid.o mq_test.o

all: CfgMgr

CfgMgr : $(CFGMGR_OBJS)
	$(CC)  $(CFGMGR_OBJS) $(CFGMGR_PATH)/lib/$(ARCH)/libmxml.a -lsqlite3 -lrt -lpthread -L$(CFGMGR_PATH)/lib/$(ARCH)/ -o $@
	cp message.o share.o ./lib/mips64el/
	cp CfgMgr /home/chenxu/LOONGSON-2k1000/src/web/boa/
	cp CfgMgr ../webDebugWithZhang/
	cp CfgMgr /home/chenxu/mount/tftproot/bin/
	cp CfgMgr /home/chenxu/LOONGSON-2k1000/toolchain/buildroot/output/target/usr/sbin/
	rm -vf *.o
    
feedWatchDog : ./feedWatchDog/feedWatchDog.o
	$(CC) ./feedWatchDog/feedWatchDog.c -o ./feedWatchDog/feedWatchDog
	cp ./feedWatchDog/feedWatchDog /home/chenxu/LOONGSON-2k1000/toolchain/buildroot/output/target/usr/sbin/
	cp feedWatchDog/feedWatchDog /home/chenxu/mount/tftproot/bin/
	rm ./feedWatchDog/feedWatchDog
	rm ./feedWatchDog.o
    
dumpcap : *
	cd ./dumpcap
	./cpTo
	cd /home/chenxu/LOONGSON-2k1000/toolchain/buildroot/output/build/wireshark-2.6.4/
	make
	./cpTo_dumpcap 
    
mq_test: $(MQTEST_OBJS)
	$(CC) $(MQTEST_OBJS)  $(INC) $(CFGMGR_PATH)/lib/$(ARCH)/libmxml.a -lsqlite3 -lrt -lpthread -L$(CFGMGR_PATH)/lib/$(ARCH)/ -o mq_test 
	cp mq_test /home/chenxu/mount/tftproot/bin/
	cp message.o share.o ./lib/mips64el/
	rm -vf *.o

.c.o:
	echo Compiling $<
	$(CC) $(CFLAGS) $(INC) -c $<
    
clean:
	rm -vf *.o
	rm ./feedWatchDog/feedWatchDog

