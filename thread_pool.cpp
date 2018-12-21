#include <iostream>

#include "thread_pool.h"

bool thread_pool::m_shutdown = false;

thread_pool::thread_pool() :
    m_inited(false),
    m_capacity(0),
    m_queues(NULL),
    m_threads(NULL),
    m_queue_index(-1)
{

}

thread_pool::~thread_pool()
{

}

bool thread_pool::create(int pool_capacity, int queue_capacity)
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

    if(pthread_mutex_init(&m_mutex, NULL) != 0)
    {
        return false;
    }

    m_threads = new pthread_t[m_capacity];
    if(!m_threads)
    {
        free();
        return false;
    }

    m_queues = new async_eventfd_queue[m_capacity];
    if(!m_queues)
    {
        free();
        return false;
    }

    for(int i = 0; i < m_capacity; i++)
    {
        if(!m_queues[i].create(queue_capacity))
        {
            free();
            return false;
        }
            
        if(pthread_create(&m_threads[i], NULL, &thread_pool::run, &(m_queues[i])) != 0)
        {
            free();
            return false;
        }
    }

    m_inited = true;
    m_queue_index = 0;

    std::cout << "create thread pool success." << std::endl;

    return true;
}

void thread_pool::destroy()
{
    if(!m_inited)
    {
        return;
    }
    
    m_shutdown = true;
    for(int i = 0; i < m_capacity; i++)
    {
        pthread_join(m_threads[i], NULL);
        m_threads[i] = -1;
    }

    free();
    m_inited = false;
    m_shutdown = false;
    m_queue_index = -1;
}

bool thread_pool::add_task(const task_fuction_t func, void *arg)
{
    if(!m_queues || !func || m_queue_index < 0 || m_shutdown)
    {
        std::cout << "queue index: " << m_queue_index << std::endl;
        return false;
    }

    task_t task;
    task.run = func;
    task.arg = arg;

    pthread_mutex_lock(&m_mutex);
    //std::cout << "current queue index: " << m_queue_index << std::endl;
    if(!m_queues[m_queue_index].push(&task))
    {
        std::cout << "add task failed." << std::endl;
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    //std::cout << "add task to queue: " << m_queue_index << std::endl;

    m_queue_index++;
    m_queue_index %= m_capacity;

    pthread_mutex_unlock(&m_mutex);
    return true;    
}

void thread_pool::free()
{
    if(m_queues)
    {
        delete[] m_queues;
        m_queues = NULL;
    }

    if(m_threads)
    {
        delete[] m_threads;
        m_threads = NULL;
    }

    pthread_mutex_destroy(&m_mutex);
}

void* thread_pool::run(void *arg)
{
    if(!arg)
    {
        std::cout << "thread exit id: " << pthread_self() << std::endl;
        return NULL;
    }

    async_eventfd_queue *queue = (async_eventfd_queue *)arg;
    if(!queue)
    {
        return NULL;
    }
    
    while(!m_shutdown || !queue->empty())
    {
        task_t *task = queue->front(50);
        if(!task)
        {
            continue;
        }

        if(!(task->run))
        {
            //std::cout << "not run." << std::endl;
            continue;
        }
        
        task->run(task->arg);
        queue->pop();
    }
}

