#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<errno.h>
//定义两个FIFO有名管道
#define FIFO_OUT "ui"
#define FIFO_IN "phone!"
#define BUF_MAX_LEN 64
//定义通话状态
enum TASK_PHONE_STATE
{
	        TASK_PHONE_STATE_NONE = 0,
                TASK_PHONE_STATE_RING,
                TASK_PHONE_STATE_TALK,
                TASK_PHONE_STATE_HANGUP,
};
//非阻塞检测键盘输入
static int get_char()
{
	fd_set rfds;
	struct timeval tv;
	int ch = 0;

	FD_ZERO(&rfds);
	FD_SET(0,&rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 1;//设置等待超时事件
	if(select(1,&rfds,NULL,NULL,&tv)>0)
	{
		ch = getchar();
	}
	return ch;
}
//向有名管道中写入信息
int write_FIFO(int argc, char *argv)
{
    int fd_t = -1;
    char buf_t[BUF_MAX_LEN] = {0};
    int len_t = 0;
    //  创建管道
    if(access(FIFO_OUT, F_OK) != 0)
    {
        if(mkfifo(FIFO_OUT, 0666) < 0)
        {
            perror("can not create");
            exit(1);
        }

    }
//  step 1打开管道
    fd_t = open(FIFO_OUT,O_WRONLY | O_NONBLOCK,0);
    if(fd_t == -1)
    {
        perror("open FIFO_OUT");
        return -1;
    }
//  step2写入信息
    strcpy(buf_t,argv);

//  step3发送信息
    if((len_t = write(fd_t, buf_t, sizeof(buf_t))) == -1)
    {
        printf("has not been read yet. Try again");
    }
    else
    {
        printf("write %s to the FIFO\n",buf_t);
    }

//  step 4关闭管道
    close(fd_t);
    return 0;
}

int main()
{
    int fd = -1;
    char buf[BUF_MAX_LEN] = {0};
    int len = 0;
    int ch = 0;

//  step 1创建管道
    if(access(FIFO_IN, F_OK) != 0)
    {
        if(mkfifo(FIFO_IN, 0666) < 0)
        {
            perror("can not create");
            exit(1);
        }

    }

//  step 2打开管道,设置非阻塞
    if((fd = open(FIFO_IN,O_RDONLY|O_NONBLOCK,0)) < 0)
    {
        perror("open fifo");
        exit(1);
    }
//  step 3循环读取消息
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        if((len = read(fd,buf,sizeof(buf))) > 0)
        {
            printf("phone is hang up");
            break;
        }
        else
        {
            ch = get_char();
            if(ch == 'q')
            {
                write_FIFO(1,"q");
                printf("send quit msg ok!");
                break;
            }
        }
       // printf("working\n");
        //usleep(100*1000);
    }
    return 0;
}