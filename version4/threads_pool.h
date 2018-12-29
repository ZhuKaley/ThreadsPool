#ifndef __THREADS_POOL_H__
#define __THREADS_POOL_H__

#define MAX_THREAD_COUNT 1024

class thread;
class threads_pool
{
public:
    threads_pool();
    ~threads_pool();

	bool init(const int count);
	void deinit();

	bool add_task(const task_t* t);

private:
	bool polling(const task_t* t);

private:
	thread *m_threads;
	int m_capacity;
};

#endif
