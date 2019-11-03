include ../config.mk
LIB=-lrt -pthread libcgic.a
SRCS=msg.c share.c
OBJS=$(SRCS:.c=.o)
CFLAGS=-g -Wall -O0


all: libcgic.a cfgMgrCgi.cgi

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

cgic.o:cgic.c cgic.h
	$(CC) -c cgic.c

cfgMgrCgi.cgi: cfgMgrCgi.c libcgic.a 
	$(CC) $(CFLAGS) $(INC) cfgMgrCgi.c -o cfgMgrCgi.cgi $(LIB)	
	cp cfgMgrCgi.cgi /home/chenxu/LOONGSON-2k1000/src/web/cgi-bin/
	cp cfgMgrCgi.cgi ../../webDebugWithZhang/
	cp cfgMgrCgi.cgi /home/chenxu/mount/tftproot
	rm -vf *.o
	rm -vf capture

clean:
	rm -vf *.o
	rm -vf *.a
	rm -vf *.cgi
	rm -vf capture
	
%.o:%.c  
	$(CC) $(CFLAGS) $(DEVMGR_INC) -o $@ -c $<
