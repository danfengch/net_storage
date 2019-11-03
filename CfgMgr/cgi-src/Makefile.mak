CC=gcc
#CFLAGS=-Wall -std=gnu99 
AR=ar
RANLIB=ranlib
INC=../app/devmgr/ ../app/ptl/
CFLAGS = -g -Wall -O0 -I../app/devmgr/ -I../app/ptl/ -I../app/inc
LIB=-lrt -pthread libcgic.a
SRCS=msg.c share.c
OBJS=$(SRCS:.c=.o)
#CFLAGS += -I $(DEVMGR_INC)


all: libcgic.a sys_mgr.cgi user_mgr.cgi import_file.cgi add_io_dev.cgi remove_io_dev.cgi capture
	#cp *.cgi $(CGI_BIN)

#install: libcgic.a
	cp *.cgi ../../tftpboot/
	cp capture ../../tftpboot
	#cp cgic.h /usr/local/include
	#@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

cgic.o:cgic.c cgic.h
	$(CC) -c cgic.c
#mingw32 and cygwin users: replace .cgi with .exe

sys_mgr.cgi: sys_mgr.c libcgic.a
	$(CC) sys_mgr.c -o sys_mgr.cgi libcgic.a

user_mgr.cgi: user_mgr.c libcgic.a
	$(CC) user_mgr.c -o user_mgr.cgi libcgic.a
  
import_file.cgi: import_file.c libcgic.a
	$(CC) import_file.c -o import_file.cgi libcgic.a

capture: capture.c libcgic.a
	$(CC) capture.c -o capture libcgic.a
	
io_actor.c.cgi: io_actor.c.c libcgic.a $(OBJS)
	$(CC) $(CFLAGS) io_actor.c.c $(OBJS) -o io_actor.cgi libcgic.a $(LIB)

#remove_io_dev.cgi: remove_io_dev.c libcgic.a $(OBJS)
#	$(CC) $(CFLAGS) remove_io_dev.c $(OBJS) -o remove_io_dev.cgi libcgic.a $(LIB)

msg.o:msg.c msg.h
	$(CC) $(CFLAGS) -c msg.c
share.o:share.c share.h
	$(CC) $(CFLAGS) -c share.c


#msg.o: msg.c msg.h
#	$(CC) $(CFLAGS) msg.c -c
#share.o:share.c share.h
#	$(CC) $(CFLAGS) share.c -c
clean:
	rm -vf *.o
	rm -vf *.a
	rm -vf *.cgi
	rm -vf capture
	
#%.o:%.c  
	#$(CC) $(CFLAGS) $(DEVMGR_INC) -o $@ -c $<
