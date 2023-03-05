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
//设置时间间隔
#define TIME_NEXT 50
//定义信号
#define SIG_UI_QUIT 35
#define SIG_PHONE_QUIT 36
//定义两个FIFO有名管道
#define FIFO_IN "ui"
#define FIFO_OUT "phone!"
#define BUF_MAX_LEN 64
//定义通话状态
enum TASK_PHONE_STATE
{
	        TASK_PHONE_STATE_NONE = 0,
                TASK_PHONE_STATE_RING,
                TASK_PHONE_STATE_TALK,
                TASK_PHONE_STATE_HANGUP,
};
int phone_state = TASK_PHONE_STATE_NONE;
void set_state(int state)
{
	phone_state = state;
}
//获取通话状态
int get_state(void)
{
	return phone_state;
}
int get_ui_pid()
{
	int pid = -1;
	FILE *fp = NULL;
	char buf[12]={0};
	//打开管道
	fp = popen("ps -e | grep \'task_ui_pipe\' | awk \'{print $1}\'","r");
	fgets(buf,sizeof(buf),fp);
	if(strlen(buf)>0)
	{
		pid = atoi(buf);
	}
	return pid;
}
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
	int time =0;

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
    printf("jin ru xun huan\n");
// step 3循环读取
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        if((len = read(fd,buf,sizeof(buf))) > 0)
        {
            if(buf[0] == 'q')
            {
                set_state(TASK_PHONE_STATE_HANGUP);
            }
        }
        time++;//依据时间变更状态
		if(time>=TIME_NEXT)
        {
            time = 0;
            if(get_state() == TASK_PHONE_STATE_RING)
			{
				set_state(TASK_PHONE_STATE_TALK);
			}
			else if(get_state() == TASK_PHONE_STATE_TALK)
			{
				set_state(TASK_PHONE_STATE_HANGUP);
			}
			else
			{
				set_state(TASK_PHONE_STATE_RING);
			}
			printf("Current state is %d!\n",get_state());

        }
        //检测状态是否为挂断
        if(get_state() == TASK_PHONE_STATE_HANGUP)
        {
            break;
        }
        usleep(100*1000);
    }
    close(fd);
    write_FIFO(1,"q");
    printf("send quit msg ok!");





    return 0;
}