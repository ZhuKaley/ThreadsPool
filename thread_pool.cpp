#include "thread_pool.h"

thread_pool::thread_pool() :
    m_inited(false),
    m_capacity(0)
    m_queues(NULL),
    m_threads(NULL)
{

}

thread_pool::~thread_pool()
{

}

bool thread_pool::init(int pool_capacity, int queue_capacity)
{
    if(m_inited)
    {
        return true;
    }
    
    if(pool_capacity <= 0)
    {
        pool_capacity = DEFAULT_CAPACITY;
    }

    m_capacity = pool_capacity;

    m_threads = new pthread_t[m_capacity];
    if(!m_threads)
    {
        return false;
    }

    m_queues = new queue[m_capacity];

    for(int i = 0; i < m_capacity; i++)
    {
        if(pthread_create(&m_threads[i], NULL, &thread_pool::run, NULL) != 0)
        {
            free();
            return false;
        }

        if(!m_queues[i].create(queue_capacity))
        {
            free();
            return false;
        }
    }

    return true;
}

void thread_pool::deinit()
{

}

bool thread_pool::add(const task_t *task)
{

}

void thread_pool::free()
{

}

void* thread_pool::run(void *arg)
{

}

