#ifndef __THREADS_POOL_H__
#define __THREADS_POOL_H__

#include "base_async_queue.h"

#define MAX_POOL_CAPACITY 128
#define MAX_QUEUE_CAPACITY 65535


typedef struct pool
{
    bool started;
    pthread_t *threads;
    base_async_queue *queue;
    int capacity;
    int queue_capacity;
    int shutdown;

    pool() :
        started(false),
        capacity(0),
        queue_capacity(0),
        shutdown(0)
    {

    }
} pool_t;

class threads_pool
{
public:
    typedef void* (*task_function_t)(void *);
    
    threads_pool();
    ~threads_pool();

    bool create(int pool_size, int queue_size);
    bool destroy();

    bool add_task(task_function_t func, void *arg);

private:
    void free();
    static void* run(void *obj);

private:
    pool_t *m_pool;
};

#endif
