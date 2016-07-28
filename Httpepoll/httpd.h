#pragma once

#include<stdio.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<pthread.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<netinet/in.h>
#include<sys/sendfile.h>
#include<fcntl.h>
#include<sys/stat.h>

#define _SIZE_ 1024
#define _SIZE_MOTHOD_ 20
#define _SIZE_URL_ 2048
#define _MAX_EPFD_ 1024

typedef struct thread_work{
	void*(*httpserver)(void *arg);
	void *arg;
	struct thread_work *next;
}thread_work;



typedef struct thread_pool
{
	pthread_mutex_t lock;
	pthread_cond_t cond;
	struct thread_work *head;
	pthread_t *threadth;
	int shutdown;
	int max_thread_num;
	int wait_num;
}thread_pool;


void* thread_run(void *arg);
void pool_init(int max_thread_num);
int add_work(void*(*httpserver)(void*arg),void *arg);
int pool_destroy();

void usage(char *proc);
void printf_log(const char *function,const int line,const char *file);
void clear_head(int sock);
int get_line(int fd,char *line,int size);
int startup(char *ip,int port);
void ret_cgi(int sock,char *method,char*path,char*query_string);
void ret_html(int sock,char *mothod,const char *path,int size);
