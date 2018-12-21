#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <pthread.h>

#include "queue.h"
#include "async_eventfd_queue.h"

#define DEFAULT_CAPACITY 1024

class thread_pool
{
public:
    typedef void* (*task_fuction_t)(void*);
    
    thread_pool();
    ~thread_pool();

    bool create(int pool_capacity, int queue_capacity);
    void destroy();
    
    bool add_task(const task_fuction_t func, void *arg);

private:
    void free();
    static void* run(void *arg);

private:
    bool m_inited;
    int m_capacity;
    int m_queue_index;
    async_eventfd_queue *m_queues;
    pthread_t *m_threads;
    pthread_mutex_t m_mutex;
    static bool m_shutdown;
};

#endif
