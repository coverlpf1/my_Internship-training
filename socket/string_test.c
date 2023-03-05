#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
 
 #define BUF_SIZE 1024

 char res[BUF_SIZE] = {0};

 pthread_t tid = 0 ; 

 void *check(void *arg)
 {
    while(1)
    {
            while(strlen(res) != 0)
        {
            printf("now the res is%s \n",res);
            memset(res,0,sizeof(res));
            sleep(2);
        }
    }
 }

 int main()
 {
    pthread_create(&tid,NULL,check,NULL);
    while(1)
    {
        scanf("%s",res);
    }
 }
