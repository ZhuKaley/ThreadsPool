#ifndef __THREAD_H__
#define __THREAD_H__

#include <queue>
#include <string>
#include <pthread.h>
#include <sys/epoll.h>

typedef struct task
{
	task() :
		arg(NULL),
		run(NULL)
	{

	}
	
	void* arg;
	void* (*run)(void *);

	//test
	~task()
	{
		//std::cout << "destructing..." << std::endl;
	}
} task_t;

class thread
{
public:
    thread();
    ~thread();

	bool init();
	void deinit();

    bool add_task(const task_t *t);

private:
	static void* run(void *arg);

private:
	bool m_inited;
	bool m_run_enable;
	std::queue<const task_t*> m_q;
	pthread_t m_thread_id;
	int m_epfd;	//epoll fd
	int m_evfd;	//eventfd
};

#endif
