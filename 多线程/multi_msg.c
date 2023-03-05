#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<pthread.h>
//#作者：LPF（李鹏飞）
//#修改内容：在原多线程下载队列基础上，增加消息队列用于主进程向处理线程发送信息
//处理线程根据消息类型进行划分，待各类消息完善，便使用task_add函数向下载队列添加任务
//新线程：msgrec_pthread,需求参数为long类型（int类型会触发内存空间大小不一的warning）id号，即消息队列标识码
//新函数：msg_snd，用于向消息队列中写入信息，位于main函数上方，设计参数为：发送内容，消息类型，消息队列iD
//定义消息结构体
#define MSG_DATA_LEN 128
struct message{
	long msg_type;
	char msg_data[MSG_DATA_LEN];
};
enum Name_of_DuiLie{
    down_path = 1,
    save_path,
    prio,
    end,
};

pthread_t tid_t=1;
pthread_t tid = 0 ;
static pthread_mutex_t task_slist_mutex;
#define MAX_URL_LEN  (200)
#define MAX_PATH_LEN  (200)
#define MAX_TASK_CNT  (10)

typedef struct
{
    void** items;
    int item_maxcount;
    int item_count;
    int item_size;
}__Z_SLIST;

typedef void* Z_SLIST;

typedef struct _DOWNLOAD_TASK
{
    char url[MAX_URL_LEN];
    char save_path[MAX_PATH_LEN];
}__download_task;

static Z_SLIST task_slist = NULL;

typedef enum
{
    PRIO_NORMAL,
    PRIO_HIGH
} DOWNLOAD_PRIO;

int task_slist_insert(Z_SLIST list, const void* item, int idx);
Z_SLIST task_slist_new(int item_size, int item_maxcount);//创建下载队列
int task_slist_delete(Z_SLIST list, int idx);//删除下载队列
int task_slist_add(Z_SLIST list, const void* item);
void* task_slist_get(Z_SLIST list, int idx);
int task_add(char* url, char* save_path, DOWNLOAD_PRIO prio);
void *download_thread(void *arg);
void *msgrcv_pthread(void *arg);
int init_service(void);
int deinit_service(void);
void msg_snd(char *res,long msg_type,long id);

void *msgrcv_pthread(void *arg)
{
    struct message msg_t;
    long id = (long)arg;
    char download_path[MSG_DATA_LEN] = {0};
    char saveing_path[MSG_DATA_LEN] = {0};
    long prio_type = PRIO_NORMAL;
    int flag1 = 0, flag2 = 0, flag3 = 0;
    while(1)
    {
        
            memset(&msg_t,0,sizeof(msg_t));

            if(msgrcv(id,&msg_t,MSG_DATA_LEN,0,IPC_NOWAIT)>= 0)
		    {
			    if(msg_t.msg_type == down_path)
			    {
				   printf("下载路径为%s\n",msg_t.msg_data);
                   strcpy(download_path,msg_t.msg_data);

                   flag1++;
			    }
				else if(msg_t.msg_type == save_path)
				{
                    printf("保存路径为%s\n",msg_t.msg_data);
					strcpy(saveing_path,msg_t.msg_data);
                    flag2++;
				}
                else if(msg_t.msg_type == prio)
                {
                    if(msg_t.msg_data == "1")
                    {
                        prio_type = PRIO_HIGH;
                    }
                    else{
                        prio_type = PRIO_NORMAL;
                    }
                    
                    flag3++;
                }
                else
                {
                    break;
                }
		    }
            if(flag1 == 1 && flag2 == 1 && flag3 ==1)
            {
                task_add(download_path,saveing_path,prio_type);
                prio_type = PRIO_NORMAL;
                memset(download_path,0,sizeof(download_path));
                memset(saveing_path,0,sizeof(saveing_path));
                flag1=0;
                flag2=0;
                flag3=0;
            }
			

    }
    printf("线程退出\n");
    return NULL;


}
int task_slist_insert(Z_SLIST list,const void* item, int idx)
{
    __Z_SLIST* l = (__Z_SLIST*)list;

    if(l->item_count >= l->item_maxcount)
    {
        return -1;
    }

    if(idx >= 0 && idx <= l->item_count)
    {
        int i = 0;
        for(i = l->item_count; i> idx; i--)
        {
            memcpy((char*)l->items + i * l->item_size,
                   (char*)l->items + (i-1)*l->item_size, (int)l->item_size);       
        }
        memcpy((char*)l->items + idx * l->item_size, item, (int)l->item_size);
        l->item_count++;
    }
    return idx;
}

Z_SLIST task_slist_new(int item_size, int item_maxcount)
{
    __Z_SLIST* list = NULL;
    int itemstotalsize = item_size * item_maxcount;

    list = (__Z_SLIST*)malloc(sizeof(__Z_SLIST));
    list->items = (void**)malloc(itemstotalsize);

    list->item_count = 0;
    list->item_maxcount = item_maxcount;
    list->item_size = item_size;

    return (Z_SLIST)list;
}

int task_add(char* url, char* save_path, DOWNLOAD_PRIO prio)
{
    int ret = -1;
    __download_task task = {0};
    strncpy(task.url, url, sizeof(task.url));
    strncpy(task.save_path, save_path, sizeof(task.save_path));
    pthread_mutex_lock(&task_slist_mutex);
    if(prio == PRIO_HIGH)
    {
        ret = task_slist_insert(task_slist, &task, 0);
    }
    else
    {
        ret = task_slist_add(task_slist, &task);
    }
    pthread_mutex_unlock(&task_slist_mutex);

    return ret;
}

int task_slist_add(Z_SLIST list, const void* item)
{
    __Z_SLIST* l = (__Z_SLIST*)list;

    if(l->item_count >= l->item_maxcount)
    {
        return -1;
    }

    if(l->item_count < l->item_maxcount)
    {
        memcpy((char*)l->items + l->item_count * l->item_size, item, (int)l->item_size);
        l->item_count++;
    }
    
    return 0;
}

void* task_slist_get(Z_SLIST list, int idx)
{
    __Z_SLIST* l = (__Z_SLIST*)list;

    if(!(idx >= 0 && idx < l->item_count))
    {
        return NULL;
    }

    return (void*)( (char*)l->items + idx * l->item_size);
}

int task_slist_delete(Z_SLIST list, int idx)
{
    __Z_SLIST* l = (__Z_SLIST*)list;

    if(idx >= 0 && idx < l->item_count)
    {
        int i = 0;

        for( i = idx; i < l->item_count - 1; i++)
        {
            memcpy((char*)l->items + i * l->item_size,
                    (char*)l->items + (i+1)*l->item_size, (int)l->item_size);             
        }
        l->item_count--;
    }

    return 0;
}
void *download_thread(void *arg)
{
    __download_task* task = NULL;
    while(1)
    {
        pthread_mutex_lock(&task_slist_mutex);
        task = (__download_task*)task_slist_get(task_slist,0);
        if(task != NULL)
        {
            printf("New download task:\n");
            printf("Downloading ... url=%s\n",task->url);
            sleep(1);
            printf("Finish! save_path=%s\n\n", task->save_path);
            task_slist_delete(task_slist,0);
        }
        pthread_mutex_unlock(&task_slist_mutex);
        sleep(1);
    }
    return NULL;
}

int init_service(void)
{
    task_slist = task_slist_new(sizeof(__download_task), MAX_TASK_CNT);
    pthread_create(&tid,NULL,download_thread,NULL);
    return tid;
}

int deinit_service(void)
{
    int ret = -1;
    ret = pthread_cancel(tid);
    if(0 == ret)
    {
        pthread_join(tid, NULL);
    }
    return ret;
}

void msg_snd(char *res,long msg_type,long id)
{

        struct message msg;//创建结构体
        memset(&msg,0,sizeof(msg));
        strcpy(msg.msg_data,res);
        msg.msg_type = msg_type;
        msgsnd(id,&msg,MSG_DATA_LEN,0);
        printf("写入成功\n");
}
int main()
{
    if(pthread_mutex_init(&task_slist_mutex,NULL) != 0)
    {
        printf("Init mutex error.\n");
        return 1;
    }
    init_service();//初始化下载服务
    long id=-1;//创建ID号
    //第一步创建队列
    if((id = msgget(2000,0666 | IPC_CREAT)) == -1)
	{
		perror("msgget");
		exit(1);
	}
    pthread_create(&tid_t,NULL,msgrcv_pthread,(void *)id);//创建接收线程

    //向消息队列中写入信息
    //写入下载路径
    msg_snd("http://video1",down_path,id);

    sleep(1);
//写入存储路径
    msg_snd("/sdcard/video1",save_path,id);
    sleep(1);
//写入优先级别
    msg_snd("1",prio,id);

        //向消息队列中写入信息
    //写入下载路径
    msg_snd("http://video2",down_path,id);

    sleep(1);
//写入存储路径
    msg_snd("/sdcard/video2",save_path,id);
    sleep(1);
//写入优先级别
    msg_snd("0",prio,id);




    //task_add("http://video1", "/sdcard/video1", PRIO_NORMAL);
    //task_add("http://video2", "/sdcard/video2", PRIO_NORMAL);

    sleep(1);

    task_add("http://video3", "/sdcard/video3", PRIO_HIGH);
    sleep(5);
    msg_snd("",end,id);//关闭线程
    pthread_join(tid_t,NULL);
    deinit_service();//反初始化下载服务

    return 0;
}