#include "thread_pool_pv.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static int tp_alive = 1;

thread_pool *tp_init(int thread_num)
{
	thread_pool *tp;
	int i;
	if(thread_num < 1)
		thread_num = 1;
	tp = (thread_pool *)malloc(sizeof(thread_pool));
	if(NULL == tp)
	{
		printf("Error:allocate memory for thread_pool failed\n");
		return NULL;
	}
	tp->thread_num = thread_num;
	tp->threads = (pthread_t*)malloc(thread_num * sizeof(pthread_t));
	if(NULL == tp->threads)
	{
		printf("ERROR:allocate memory for threads in thread pool failed\n");
		return NULL;
	}
	if(tp_job_queue_init(tp))
		return NULL;
	tp->job_queue->queue_sem = (sem_t *)malloc(sizeof(sem_t));
	sem_init(tp->job_queue->queue_sem,0,0);

	for( i = 0;i<thread_num;i++)
	{
		pthread_create(&(tp->threads[i]),NULL,(void*)tp_thread_func,(void *)tp);
	}
	return tp;
}

int tp_job_queue_init(thread_pool *tp)
{
	tp->job_queue = (thread_pool_job_queue*)malloc(sizeof(thread_pool_job_queue));
	if(NULL == tp->job_queue)
		return -1;
	tp->job_queue->head = NULL;
	tp->job_queue->tail = NULL;
	tp->job_queue->num = 0;
}

void *tp_thread_func(thread_pool *tp)
{
	FUNC function;
	void *arg_buf;
	thread_pool_job *job_p;

	while(tp_alive)
	{
		if(sem_wait(tp->job_queue->queue_sem))
		{
			printf("thread waiting for semphone...\n");
			exit(1);
		}
		if(tp_alive)
		{
			pthread_mutex_lock(&mutex);
			job_p = tp_get_lastjob(tp);
			if(NULL == job_p)
			{
				pthread_mutex_unlock(&mutex);
				continue;
			}
			function = job_p->function;
			arg_buf = job_p->arg;
			if(tp_delete_lastjob(tp))
				return;
			pthread_mutex_unlock(&mutex);
			printf("consumer ... get a job from job queue and run it\n");
			function(arg_buf);
			free(job_p);
		}
		else
			return;
	}
	return;
}