#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "queue.h"

#define DEFAULT_CAPACITY 1024

class thread_pool
{
public:
    thread_pool();
    ~thread_pool();

    bool init(int pool_capacity, int queue_capacity);
    void deinit();
    
    bool add(const task_t *task);

private:
    void free();
    static void* run(void *arg);

private:
    bool m_inited;
    int m_capacity;
    queue *m_queues;
    pthread_t *m_threads;
};

#endif
