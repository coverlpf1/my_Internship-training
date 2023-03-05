#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>


#define SEND_BUF_SIZE 100
#define RECV_BUF_SIZE 100
#define LISTEN_PORT 7801
pthread_t tid = 0;

void *udp(void *arg)
{
    //服务器监听的SOCKET
    int listen_sock = 0;

    //本机socket地址
    struct sockaddr_in hostaddr;
    //客户端socket地址
    struct sockaddr_in clientaddr;

    //接收数据内存
    char recvbuf[RECV_BUF_SIZE] = {0};

    memset((void*)&hostaddr,0,sizeof(hostaddr));
    memset((void*)&clientaddr,0,sizeof(clientaddr));

    //设置本机的socket地址
    hostaddr.sin_family = AF_INET;
    hostaddr.sin_port = htons(LISTEN_PORT);//涉及大端小端切换
    hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //创建Socket
    listen_sock = socket(AF_INET,SOCK_DGRAM,0);
    if(listen_sock < 0)
    {
        printf("create socket failed \n");
        exit(1);
    }

    //绑定socket
    if(bind(listen_sock,(struct sockaddr*)&hostaddr,sizeof(hostaddr))<0)
    {
        printf("bind socket failed\n");
        close(listen_sock);
        exit(1);
    }

    while(1)
    {
        int iRealLength = recv(listen_sock,recvbuf,RECV_BUF_SIZE,0);

        if(iRealLength < 0)
        {
            printf("receive data error \n");
            close(listen_sock);
            exit(1);
        }

        printf("receive data is : %s length : %d \n",recvbuf,iRealLength);
    }

    //关闭监听的socket
    close(listen_sock);
}
int main(int argc,char *argv[])
{
    int time = 0;
    pthread_create(&tid,NULL,udp,NULL);
    while(1)
    {
        printf("now time is%d\n",time);
        time += 5;
        sleep(5);
    }
    
    return 0;
}