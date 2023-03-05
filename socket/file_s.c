#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#define RECEIVER_BUF_SIZE 1024
#define SERVER_PORT 7800
#define SEND_BUF_SIZE 100
#define SERVER_PORT_UDP 7801
pthread_t file_tid = 0;
pthread_t udp_tid = 1;
void *udp_snd(void *arg)
{
    //定义socket
    int sock_fd = 0;
    //定义发送的数据内存
    char sendbuf[SEND_BUF_SIZE] = {0};
    //定义接收数据长度
    int sendlen = 0;

    //定义服务器的socket地址
    struct sockaddr_in ser_addr;

    //设置服务器的socket参数
    memset(&ser_addr,0,sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1",(struct in_addr*)&ser_addr.sin_addr);
    ser_addr.sin_port = htons(SERVER_PORT_UDP);

    //创建socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        printf("create socket failed \n");
        exit(1);
    }
    char ch[SEND_BUF_SIZE] = {0};
    while(1)
    {
        memset(ch,0,sizeof(ch));
        scanf("%s",ch);
        if(ch[0] == 'q' && ch[1] == 'u' )
        {
            break;
        }
        //发送数据
        //sprintf(sendbuf,"hello server");
        strcpy(sendbuf,ch);
        sendlen = strlen(sendbuf) + 1;
        int iRealSend = sendto(sock_fd,sendbuf,sendlen,0,
            (struct sockaddr*)(&ser_addr),sizeof(struct sockaddr_in));
            
        printf("send data is:%s,send length: %d\n",sendbuf,iRealSend);
        sleep(1);
    }
    
    //关闭socket
    close(sock_fd);
}
void *file_recv(void *arg)
{
    while(1)
    {
        //定义socket
        int sock_fd = 0;
        //定义接收文件名称的内存
        char file[RECEIVER_BUF_SIZE] = {0};
        //定义接收数据的内存
        char buf[RECEIVER_BUF_SIZE] = {0};

        //定义服务器的socket通讯地址
        struct sockaddr_in ser_addr;

        //设置服务器的socket参数
        memset(&ser_addr,0,sizeof(ser_addr));
        ser_addr.sin_family = AF_INET;
        inet_aton("127.0.0.1",(struct in_addr*)&ser_addr.sin_addr);
        ser_addr.sin_port = htons(SERVER_PORT);

        //创建socket
        sock_fd = socket(AF_INET,SOCK_STREAM,0);
        if(sock_fd < 0)
        {
            printf("create socket failed\n");
            exit(1);
        }

        //连接服务器
        if(connect(sock_fd,(struct sockaddr*)&ser_addr,sizeof(ser_addr)) < 0)
        {
            printf("connect socket failed\n");
            close(sock_fd);
            exit(1);
        }
        //接收文件名称
        int receivelength = 0;
        while(receivelength <= 0)
        {
            receivelength = read(sock_fd,file,sizeof(file));
        }
        printf("receive name length:%d\n",receivelength);
        sprintf(buf,"%s_%s","receive",file);
        printf("begin receive file %s\n",file);

        //创建接收文件
        FILE *target;
        char path[300] = {0};
        strcpy(path,"receive/");
        strcat(path,buf);
        target = fopen(path,"wb");
        if(target == NULL)
        {
            printf("open file failed\n");
            close(sock_fd);
            exit(1);
        }

        //通知服务器准备好了
        sprintf(buf,"ready");
        write(sock_fd,buf,strlen(buf));

        //开始接收文件内容
        memset(buf,0,RECEIVER_BUF_SIZE);
        while((receivelength = read(sock_fd,buf,RECEIVER_BUF_SIZE)) > 0)
        {
            if(receivelength < 0)
            {
                printf("receive file error\n");
                fclose(target);
                close(sock_fd);
                exit(1);
            }

            printf("receive file length:%d\n",receivelength);
            fwrite(buf,1,receivelength,target);
            memset(buf,0,RECEIVER_BUF_SIZE);
        }
        printf("receive file success\n");
        //关闭文件句柄
        fclose(target);

        //关闭socket
        close(sock_fd);
    }
}
int main()
{
    pthread_create(&file_tid,NULL,file_recv,NULL);
    pthread_create(&udp_tid,NULL,udp_snd,NULL);
    int time = 0;
    while(1)
    {
        sleep(10);
    }
    return 0;
}