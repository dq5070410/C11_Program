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
	tp->threads_num = thread_num;
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

	return 0;
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

void tp_job_queue_add(thread_pool* tp,thread_pool_job *new_job)
{
	new_job->pre = NULL;
	new_job->next = NULL;
	thread_pool_job *old_head_job = tp->job_queue->head;

	if(NULL == old_head_job)
	{
		tp->job_queue->head = new_job;
		tp->job_queue->tail = new_job;
	}
	else
	{
		old_head_job->pre = new_job;
		new_job->next = old_head_job;
		tp->job_queue->head = new_job;
	}
	++(tp->job_queue->num);
	sem_post(tp->job_queue->queue_sem);
}

thread_pool_job* tp_get_lastjob(thread_pool *tp)
{
	return tp->job_queue->tail;
}

int tp_delete_lastjob(thread_pool *tp)
{
	if(NULL == tp)
		return -1;
	thread_pool_job *last_job = tp->job_queue->tail;
	if(0 == tp->job_queue->num)
	{
		return -1;
	}
	else if(1 == tp->job_queue->num)
	{
		tp->job_queue->head = NULL;
		tp->job_queue->tail = NULL;
	}
	else
	{
		last_job->pre->next = NULL;
		tp->job_queue->tail = last_job->pre;
	}
	--(tp->job_queue->num);
	return 0;
}

int tp_add_work(thread_pool *tp,void *(func_p)(void *),void *arg)
{
	thread_pool_job *new_job = (thread_pool_job *)malloc(sizeof(thread_pool_job));
	if(NULL == new_job)
	{
		printf("Error:allocate memory for new job failed\n");
		exit(1);
	}
	new_job->function = func_p;
	new_job->arg = arg;
	pthread_mutex_lock(&mutex);
	tp_job_queue_add(tp,new_job);
	pthread_mutex_unlock(&mutex);
}

void tp_destory(thread_pool *tp)
{
	int i = 0;
	tp_alive = 0;
	for(i=0;i < tp->threads_num;++i){
		pthread_join(tp->threads[i],NULL);
	}
	free(tp->threads);
	if(sem_destroy(tp->job_queue->queue_sem))
	{
		printf("Error:destroy semaphore failed!\n");
	}
	free(tp->job_queue->queue_sem);
	thread_pool_job *current_job = tp->job_queue->tail;
	while(tp->job_queue->num)
	{
		tp->job_queue->tail = current_job->pre;
		free(current_job);
		current_job = tp->job_queue->tail;
		--(tp->job_queue->num);
	}
	tp->job_queue->head = NULL;
	tp->job_queue->tail = NULL;
}

void* thread_func1()
{
	printf("Task1 running...by thread:%u\n",(unsigned int)pthread_self());
}
void* thread_func2()
{
	printf("Task2 running...by thread:%u\n",(unsigned int)pthread_self());
}

void* thread_func_producer(thread_pool *tp)
{
	while(1)
	{
		printf("producer...add a job(job1) to job queue...\n");
		tp_add_work(tp, (void *)thread_func1, NULL);
		sleep(1);
		printf("producer...add a job(job2) to job queue...\n");
		tp_add_work(tp, (void *)thread_func2, NULL);
	}
}

int main()
{
	thread_pool *tp = tp_init(5);
	pthread_t producer_thread_id;
	pthread_create(&producer_thread_id,NULL,(void *)thread_func_producer,(void *)tp);

	pthread_join(producer_thread_id,NULL);
	tp_destory(tp);
	return 0;
}