#include <iostream>
#include <unistd.h>

#include "thread.h"

thread::thread() :
	m_inited(false),
	m_run_enable(true),
	m_thread_id(-1)
{

}

thread::~thread()
{
	deinit();
}

bool thread::init()
{
	std::cout << "in init." << std::endl;
	if(m_inited)
	{
		return true;
	}

	if(pthread_create(&m_thread_id, NULL, &thread::run, this) != 0)
	{
		std::cout << "create thread failed." << std::endl;
		return false;
	}

	std::cout << "thread id: " << m_thread_id << std::endl;
	
	return true;
}

void thread::deinit()
{
	std::cout << "in deinit." << std::endl;
	m_run_enable = false;
	if(m_thread_id >= 0)
	{
		pthread_join(m_thread_id, NULL);
		m_thread_id = -1;
	}
}

bool thread::add_task(const task_t *t)
{
	if(!t)
	{
		return false;
	}

	m_q.push(t);

	return true;
}

void* thread::run(void *arg)
{
	thread *thrd = (thread *)arg;
	
	while(thrd->m_run_enable || !thrd->m_q.empty())
	{
		if(thrd->m_q.empty())
		{
			//sleep(1);
			continue;
		}
		
		const task *t = (thrd->m_q).front();
		if(!t)
		{
			std::cout << "task is null." << std::endl;
			thrd->m_q.pop();
			continue;
		}
		
		if(t->run)
		{
			t->run(t->arg);
			(thrd->m_q).pop();
		}

		delete t;
		t = NULL;
	}
}
