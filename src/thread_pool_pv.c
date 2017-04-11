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
}