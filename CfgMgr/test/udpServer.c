/**********************************************************************
* 版权所有 (C)2016, WangSq。
*
* 文件名称：xx.c
* 文件标识：无
* 内容摘要：udpserver演示程序
* 其它说明：无
* 当前版本：V1.0
* 作    者：WangSq
* 完成日期：2016
*
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
/********************socket相关******************************************/
//socket参数
#define SERVER_PORT 8080                 /*监听端口*/
#define MAX_MSG_SIZE 256              /*recv缓冲器大小*/
int sockfd;  //套接字描述符

/*客户机的地址信息及长度信息*/
struct sockaddr_in client;
int  client_len=sizeof(struct sockaddr_in); 

//udp服务器监听函数初始化
void udpser_init()
{
   /*服务器的地址信息*/
   struct sockaddr_in server;

  /*服务器填充sockaddr server结构*/
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    server.sin_port=htons(SERVER_PORT);
   
   /*服务器建立socket描述符*/
    if(-1 == (sockfd=socket(AF_INET, SOCK_DGRAM,0)))    
        {
            perror("create socket failed");
            exit (1);
        }
    /*捆绑socket描述符sockfd*/
    if(-1 == ( bind( sockfd, ( struct sockaddr * )&server, sizeof(server) )) )
        {
            perror("bind error");
            exit (1);    
        }
}

/*接收缓冲区--------------------len =4-----------------*/
unsigned char message[MAX_MSG_SIZE];
int recv_len=0;
/* IP地址的存放缓冲区*/
char addr_p[INET_ADDRSTRLEN];
char *sndbuf = "Hello! i am OK.\n";
/**********************************************************************
* 功能描述：主函数
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期        版本号     修改人            修改内容
* -------------------------------------------------------------------
* 2016xxxx       V1.0       WangSq       创建
***********************************************************************/
int main(int argc, char *argv[])
{
    
    
    //udp服务器监听函数初始化
    udpser_init();
//    while(1)
//    {
        /********************************接收数据并打印*************************/
        recv_len=recvfrom(sockfd,message,sizeof(message),0,(struct sockaddr *)&client,&client_len); //阻塞式
        if(recv_len <0)
        {
           printf("recvfrom error\n");
           exit(1);
        }
        /*打印客户端地址和端口号*/
        inet_ntop(AF_INET,&client.sin_addr,addr_p,sizeof(addr_p));
        printf("client IP is %s, port is %d\n",addr_p,ntohs(client.sin_port));
        message[recv_len]='\0';
        /*显示消息长度*/
        printf("server received %d:%s\n", recv_len, message);

        /********************************回发数据*************************/ 
        if(sendto(sockfd,sndbuf,strlen(sndbuf),0,(struct sockaddr*)&client,client_len)<0)
        {
           printf("sendto error\n");
           exit(1);
        }
//    }
    return 0;
}