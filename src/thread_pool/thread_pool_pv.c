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