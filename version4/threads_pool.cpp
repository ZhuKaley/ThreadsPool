#include <iostream>
#include <string.h>

#include "thread.h"
#include "threads_pool.h"

threads_pool::threads_pool() :
	m_threads(NULL),
	m_capacity(0)	
{

}

threads_pool::~threads_pool()
{
	deinit();
}

bool threads_pool::init(const int count)
{
	m_capacity = count;
	if(m_capacity <= 0 || m_capacity > MAX_THREAD_COUNT)
	{
		m_capacity = MAX_THREAD_COUNT;
	}

	m_threads = new thread[m_capacity];

	for(int i = 0; i < m_capacity; i++)
	{
		if(!m_threads[i].init())
		{
			std::cout << "threads pool init failed." << std::endl;
			deinit();
			return false;
		}
	}

	return true;
}

void threads_pool::deinit()
{
	if(m_threads)
	{
		delete [] m_threads;
		m_threads = NULL;
	}
}

bool threads_pool::add_task(const task_t* t)
{
	if(!t || !m_threads)
	{
		return false;
	}
	
	int strategy = 0;
	bool ret = false;
	switch(strategy)
	{
	case 0:
		ret = polling(t);
		break;

	default:
		break;
	}

	return ret;
}

bool threads_pool::polling(const task_t* t)
{
	if(!m_threads)
	{
		return false;
	}
	
	static int curr_index = 0;
	curr_index %= m_capacity;
	return m_threads[curr_index++].add_task(t);
}