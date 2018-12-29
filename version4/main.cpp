#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#include "thread.h"
#include "threads_pool.h"

void* test(void *arg)
{
	//std::cout << "test..." << std::endl;
	static int i = 0;
	std::cout << "thread id: " << pthread_self() << ", i: " << ++i << std::endl;
}

int main(int argc, char *argv[])
{
	if(argc <= 1)
	{
		std::cout << "params invalid." << std::endl;
		return -1;
	}
	
	threads_pool pool;
	pool.init(10);
	for(int i = 0; i < atoi(argv[1]); i++)
	{
		task_t *t = new task_t;
		t->run = test;
		t->arg = NULL;
		pool.add_task(t);
	}

	sleep(10);
	return 0;
}

int main1()
{
	thread thrd;
	if(!thrd.init())
	{
		std::cout << "thread init failed." << std::endl;
		return -1;
	}

	for(int i = 0; i < 10; i++)
	{
		task_t *t = new task_t;
		t->run = test;
		t->arg = NULL;
		thrd.add_task(t);
	}

	sleep(5);	

    return 0;
}
