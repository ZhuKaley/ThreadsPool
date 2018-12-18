#include <iostream>

#include "threads_pool.h"

threads_pool::threads_pool() :
    m_pool(NULL)
{

}

threads_pool::~threads_pool()
{

}

bool threads_pool::create(int pool_size, int queue_size)
{
    if(m_pool)
    {
        return true;
    }

    m_pool = new pool_t;    
    if(pool_size <= 0)
    {
        pool_size = MAX_POOL_CAPACITY;
    }
    
    m_pool->threads = new pthread_t[pool_size];
    if(!m_pool->threads)
    {
        destroy();
        return false;
    }

    if(queue_size <= 0)
    {
        queue_size = MAX_QUEUE_CAPACITY;
    }

    m_pool->queue = new async_cond_queue();
    if(!m_pool->queue)
    {
        destroy();
        return false;
    }

    if(!m_pool->queue->create(queue_size))
    {
        destroy();
        return false;
    }

    m_pool->capacity = pool_size;
    m_pool->queue_capacity = queue_size;
    m_pool->started = true;
    m_pool->shutdown = 0;

    for(int i = 0; i < pool_size; i++)
    {
        if(pthread_create(&m_pool->threads[i], NULL, &threads_pool::run, this) != 0)
        {
            destroy();
            return false;
        }
    }

    return true;
}

bool threads_pool::destroy()
{
    if(!m_pool || !m_pool->threads || m_pool->shutdown == 1)
    {
        return true;
    }
    
    m_pool->shutdown = 1;
    for(int i = 0; i < m_pool->capacity; i++)
    {
        pthread_join(m_pool->threads[i], NULL);
    }

    free();

    return true;
}

bool threads_pool::add_task(task_function_t func, void *arg)
{
    if(!func || !m_pool || !m_pool->queue || m_pool->shutdown == 1)
    {
        return false;
    }

    task_t task;
    task.run = func;
    task.arg = arg;

    return m_pool->queue->push(&task);
}

void threads_pool::free()
{
    if(m_pool)
    {
        if(m_pool->threads)
        {
            delete[] m_pool->threads;
            m_pool->threads = NULL;
        }

        if(m_pool->queue)
        {
            delete m_pool->queue;
            m_pool->queue = NULL;
        }

        delete m_pool;
        m_pool = NULL;
    }
}

void* threads_pool::run(void* obj)
{
    if(!obj)
    {
        std::cout << "obj not exist." << std::endl;
        return NULL;
    }
    
    threads_pool *pool = (threads_pool*)obj;
    if(!pool->m_pool || !pool->m_pool->started)
    {
        std::cout << "pool not start." << std::endl;
        return NULL;
    }

    task_t task;

    std::cout << "id2: " << pthread_self() << std::endl;


    while(1)
    {
        if(pool->m_pool->shutdown == 1 && pool->m_pool->queue->empty())
        {
            break;
        }
        
        if(pool->m_pool->queue->empty())
        {
            continue;
        }

        task_t *tmp = pool->m_pool->queue->front(50);
        if(!tmp)
        {
            continue;
        }

        task.run = tmp->run;
        task.arg = tmp->arg;
        
        //pool->m_pool->queue->pop();

        task.run(task.arg);


        sleep(1);
    }
    
    return NULL;
}

