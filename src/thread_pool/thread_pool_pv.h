#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <semaphone.h>
#include <pthread.h>

//任务接口，线程调用的函数
typedef void* (*FUNC)(void *arg);

typedef struct thread_pool_job_s
{
	FUNC function;
	void *arg;
	struct thread_pool_job_s *pre;
	struct thread_pool_job_s *next;
} thread_pool_job;

typedef struct thread_pool_job_queue_s
{
	thread_pool_job *head;
	thread_pool_job *tail;
	int num;
	sem_t *queue_sem;
} thread_pool_job_queue;

typedef struct thread_pool_s{
	pthread_t *threads;
	int threads_num;
	thread_pool_job_queue *job_queue;
} thread_pool;

thread_pool* tp_init(int thread_num);

int tp_job_queue_init(thread_pool *tp);

void tp_job_queue_add(thread_pool *tp,thread_pool_job *new_job);

int tp_add_work(thread_pool *tp,void *(*func_p)(void *),void *arg);

thread_pool_job* tp_get_lastjob(thread_pool *tp);

int tp_delete_lastjob(thread_pool *tp);

void tp_destory(thread_pool *tp);

void tp_thread_func(thread_pool *tp);

void *thread_func_producer*(thread_pool *tp);
#endif