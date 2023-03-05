#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
//定义参数
//监听时间
#define MAX_LISTEN_NUM 5
//接受缓冲区最大大小
#define RECEIVE_BUF_SIZE 1024
//TCP连接端口
#define LISTEN_PORT 7800
//UDP发送与接受缓冲区
#define SEND_BUF_SIZE 100
#define RECV_BUF_SIZE 100
//UDP监听端口
#define LISTEN_PORT_UDP 7801
//两线程标识号
pthread_t file_tid = 0;
pthread_t udp_tid = 1;
//此端共有三个线程，分别为：
// 1. UFP连接发送与接受信息的端口
// 2. 建立TCP连接获取 对方Socket信息的线程
// 3. 由二号线程创建的（依据对方Socket信息）发送文件的线程
// 发送文件线程支持名称自选，发送前需要先讲文件存于send文件夹中，发送时只需输入文件名，或者位于send目录下的路径。
char res[SEND_BUF_SIZE] = {0};

void *transFile(void *sendSocket);
void *file_pth(void *arg)
{
     //服务器监听Socket
    int listen_sock = 0;
    //连接客户端的socket
    int app_sock = 0;
    //本机socket地址
    struct sockaddr_in hostaddr;
    //客户端socket地址
    struct sockaddr_in clientaddr;
    //客户端地址数据大小
    int socklen = sizeof(clientaddr);

    memset((void*)&hostaddr,0,sizeof(hostaddr));
    memset((void*)&clientaddr,0,sizeof(clientaddr));

    //设置本机的socket地址
    hostaddr.sin_family = AF_INET;
    hostaddr.sin_port = htons(LISTEN_PORT);
    hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //创建Socket
    listen_sock = socket(AF_INET,SOCK_STREAM,0);
    if(listen_sock < 0)
    {
        printf("create socket failed\n");
        exit(1);
    }

    //绑定Socket
    if(bind(listen_sock,(struct sockaddr*)&hostaddr,sizeof(hostaddr)) < 0)
    {
        printf("bind socket failed\n");
        close(listen_sock);
        exit(1);
    }

    //开始监听客户端请求
    if(listen(listen_sock,MAX_LISTEN_NUM) < 0)
    {
        printf("listen failed\n");
        close(listen_sock);
        exit(1);
    }

    //线程定义标识号
    pthread_t tid;

    while(1)
    {
        //接收客户端连接请求
        app_sock = accept(listen_sock,(struct sockaddr*)&clientaddr,(socklen_t*)&socklen);

        int ret = pthread_create(&tid,NULL,transFile,(void*)&app_sock);
        if(ret != 0)
        {
            printf("thread create error\n");
            close(app_sock);
            exit(1);
        }
    }

    //关闭监听的socket
    close(listen_sock);

    return 0;
}

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
    hostaddr.sin_port = htons(LISTEN_PORT_UDP);//涉及大端小端切换
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
        strcpy(res,recvbuf);
    }

    //关闭监听的socket
    close(listen_sock);
}

void *transFile(void *sendSocket)
{
    int sock = *(int*)sendSocket;
    unsigned char buf[1024];
    int buflen = 1024, k = 0;
    char file[256] = {0};
  //  scanf("%s",file);
    while(1)
    {
        if(strlen(res) == 0)
        {
            continue;
        }
        else
        {
            strcpy(file,res);
            memset(res,0,sizeof(res));
            break;
        }
    }
    char path[300] = {0};
    strcpy(path,"send/");
    strcat(path,file);
    FILE *source;
    source = fopen(path,"rb");
    if(source == NULL)
    {
        printf("open file failed\n");
        close(sock);
    }
    else
    {
        printf("begin tran file %s\n",file);
        //先把文件名发过去
        int iSendLen = write(sock,file,strlen(file));
        if(iSendLen < 0)
        {
            printf("send file name error\n");
            fclose(source);
            close(sock);
            return 0;
        }

        //等对方准备好
        memset(buf,0,buflen);
        int iReadLength = read(sock,buf,1024);
        printf("get string: %s, %d\n",buf,iReadLength);

        //开始发送文件内容
        memset(buf,0,buflen);
        while((k = fread(buf,1,buflen,source)) > 0)
        {
            printf("tran file Length:%d\n",k);
            //循环发送，直到文件结束
            int iSendLen = write(sock,buf,k);
            if(iSendLen < 0)
            {
                printf("send file error\n");
                break;
            }
            memset(buf,0,buflen);
        }
        printf("tran file finish\n");
        fclose(source);
        close(sock);
    }
}
int main()
{
    //创建线程以启用UDP连接，来接受消息
    pthread_create(&udp_tid,NULL,udp,NULL);
    //创建线程以启动TCP连接，来发送文件
    pthread_create(&file_tid,NULL,file_pth,NULL);
    while(1)
    {
        //保持进程存在
        sleep(1);
    }
    return 0;
}