#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<assert.h>
#include"httpd.h"

//typedef struct thread_work{
//	void*(*httpserver)(void *arg);
//	void *arg;
//	struct thread_work *next;
//}thread_work;
//
//
//
//typedef struct thread_pool
//{
//	pthread_mutex_t lock;
//	pthread_cond_t cond;
//
//	struct thread_work *head;
//	pthread_t *threadth;
//	int shutdown;
//	int max_thread_num;
//	int wait_num;
//}thread_pool;

static thread_pool*pool=NULL;

void* thread_run(void *arg)
{
	printf("start pthread %d\n",pthread_self());

	while(1)
	{
		pthread_mutex_lock(&pool->lock);
		while(pool->wait_num==0&&pool->shutdown==0)
		{
			pthread_cond_wait(&(pool->cond),&(pool->lock));
		}

		if(pool->shutdown)
		{
			pthread_mutex_unlock(&(pool->lock));
			pthread_exit(NULL);
		}

		printf("pthread is working %d\n",pthread_self());
		assert(pool->head!=NULL);
		assert(pool->wait_num!=0);

		pool->wait_num--;

		struct thread_work *worker=pool->head;
		pool->head=pool->head->next;
		pthread_mutex_unlock(&(pool->lock));

		(*(worker->httpserver))(worker->arg);
		free(worker);
		worker=NULL;
	}
	pthread_exit(NULL);
	
}


void pool_init(int max_thread_num)
{
	pool=(struct thread_pool*)malloc(sizeof(struct thread_pool));
	pthread_mutex_init(&(pool->lock),NULL);
	pthread_cond_init(&(pool->cond),NULL);
	pool->head=NULL;
	pool->shutdown=0;
	pool->max_thread_num=max_thread_num;
	pool->wait_num=0;//等待执行的任务
	
	pool->threadth=(pthread_t*)malloc(max_thread_num*sizeof(pthread_t));
	int i;
	for(i=0;i<max_thread_num;++i)
	{
		pthread_create(&pool->threadth[i],NULL,thread_run,NULL);
	}
}


int add_work(void*(*httpserver)(void*arg),void *arg)
{
	struct thread_work *worker=(struct thread_work*)malloc(sizeof(struct thread_work));
	worker->httpserver=httpserver;
	worker->arg=arg;
	worker->next=NULL;
	pthread_mutex_lock(&pool->lock);
	struct thread_work *working=pool->head;
	if(working!=NULL)
	{
		while(working->next!=NULL)
		{
			working=working->next;
		}
		working->next=worker;
	}else{
		pool->head=worker;
	}

	assert(pool->head!=NULL);

	pool->wait_num++;

	pthread_mutex_unlock(&pool->lock);

	pthread_cond_signal(&pool->cond);
}

int pool_destroy()
{
	if(pool->shutdown)
	{
		return -1;
	}
	pool->shutdown=1;

	pthread_cond_broadcast(&pool->cond);

	int i;
	for(i=0;i<pool->max_thread_num;i++)
	{
		pthread_join(pool->threadth[i],NULL);
	}
	free(pool->threadth);

	struct thread_work *phead=NULL;
	while(pool->head!=NULL)
	{
		phead=pool->head;
		pool->head=pool->head->next;
		free(phead);
	}

	pthread_mutex_destroy(&(pool->lock));
	pthread_cond_destroy(&(pool->cond));

	free(pool);
	pool=NULL;
}




//int main()
//{
//	
//	pool_init(5);
//	int worspace[20]={
//		0
//	};
//	int i;
//	for(i=0;i<20;i++)
//	{
//		add_work(httpserver,(void*)worspace[i]);
//	}
//	sleep(10);
//
//	pool_destroy();
//	return 0;
//}
