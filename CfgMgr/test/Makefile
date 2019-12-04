#CC = mips64el-linux-gcc
CC = gcc

all :        udpServer udpClient
udpserver: udpServer.c
	$(CC) udpServer.c -o udpServer
udpclient: udpClient.c
	$(CC) udpClient.c -o udpClient


clean:
	rm udpServer udpClient

.PHONY: clean
