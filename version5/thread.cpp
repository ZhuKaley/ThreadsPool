#include <iostream>
#include <unistd.h>
#include <sys/eventfd.h>

#include "thread.h"

thread::thread() :
	m_inited(false),
	m_run_enable(true),
	m_thread_id(-1),
	m_epfd(-1),
	m_evfd(-1)
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

	m_epfd = epoll_create(1);
	if(m_epfd == -1)
	{
		std::cout << "epoll_create failed." << std::endl;
		return false;
	}

	m_evfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if(m_evfd == -1)
	{
		std::cout << "eventfd failed." << std::endl;
		deinit();
		return false;
	}

	struct epoll_event ev;
	ev.data.fd = m_evfd;
	ev.events = EPOLLIN | EPOLLET;

	if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_evfd, &ev) == -1)
	{
		std::cout << "epoll add event failed." << std::endl;
		deinit();
		return false;
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
	
	if(m_epfd >= 0)
	{
		if(m_evfd >= 0)
		{
			epoll_ctl(m_epfd, EPOLL_CTL_DEL, m_evfd, NULL);
			close(m_evfd);
			m_evfd = -1;
		}

		close(m_epfd);
		m_epfd = -1;
	}
}

bool thread::add_task(const task_t *t)
{
	if(!t)
	{
		return false;
	}

	m_q.push(t);

	long buf = 1;
	return write(m_evfd, &buf, sizeof(buf)) != -1;
}

void* thread::run(void *arg)
{
	thread *thrd = (thread *)arg;

	struct epoll_event ev;

	while(thrd->m_run_enable)
	{
		int ret = epoll_wait(thrd->m_epfd, &ev, 1, 1000);
		if(ret == -1)
		{
			if(errno == EINTR)
			{
				continue;
			}
			
			std::cout << "epoll wait failed." << std::endl;
			break;
		}

		if(ret == 0)
		{
			continue;
		}

		long buf = 0;
		read(ev.data.fd, &buf, sizeof(buf));
		while(!thrd->m_q.empty())
		{
			const task_t *t = (thrd->m_q).front();
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
}
