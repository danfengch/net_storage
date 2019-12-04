/**********************************************************************
* 版权所有 (C)2016, WangSq。
*
* 文件名称：xx.c
* 文件标识：无
* 内容摘要：udpclient演示程序
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
#include <unistd.h>

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
#define UDP_TEST_PORT       8080

#define UDP_SERVER_IP       "192.168.1.226"

#define MAX_LINE             80 

int main(int argC, char* arg[])

{

    struct sockaddr_in server;

    int sockfd, len = 0;   

    int server_len = sizeof(struct sockaddr_in);     
     
    char *sendStr ="i am a client\n"; //默认发送串
    
    char buf[MAX_LINE];//接收缓冲区

    /* setup a socket，attention: must be SOCK_DGRAM */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
   
        perror("socket");
        exit(1);
    }

 
    /*complete the struct: sockaddr_in*/
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(UDP_TEST_PORT);
    server.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);

  //  while(1) {

        //printf("Please enter a string to send to server: \n");

        /* get the string from standard in*/

        //len = read(STDIN_FILENO, buf, sizeof(buf));

        /* send the string to server*/

        if(sendto(sockfd, sendStr,  strlen(sendStr), 0, (struct sockaddr *)&server, server_len)<0) //sizeof
        {
            printf("sendto error\n");
            exit(1);
        }

        memset(buf, 0, sizeof(buf));

        /* recieve the string from server*/
       
        len = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&server, &server_len);
        if(len <0)
        {
           printf("recvfrom error\n");
           exit(1);
        }
        printf("Receive from server: %s\n", buf);

  //  }

 

    return 0;

}