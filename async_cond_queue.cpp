#include <time.h>

#include "async_cond_queue.h"

async_cond_queue::async_cond_queue() :
    m_q(NULL)
{

}

async_cond_queue::~async_cond_queue()
{
    free();
}

bool async_cond_queue::create(const int capacity)
{
    if(m_q)
    {
        free();
    }

    m_q = new cond_queue;
    if(!m_q)
    {
        return false;
    }
    
    if(pthread_mutex_init(&(m_q->mutex), NULL) != 0)
    {
        return false;
    }
    
    if(pthread_cond_init(&(m_q->cond), NULL) != 0)
    {
        pthread_mutex_destroy(&(m_q->mutex));
        return false;
    }

    m_q->task_queue = new queue();
    if(!m_q->task_queue->create(capacity))
    {
        free();
        return false;
    }

    m_q->quit = 0;
    m_q->wait_threads_count = 0;
    
    return true;
}

bool async_cond_queue::destroy()
{
    if(!m_q || !m_q->task_queue)
    {
        return false;
    }
    
    pthread_mutex_lock(&(m_q->mutex));
    m_q->quit = 1;
    pthread_cond_broadcast(&(m_q->cond));
    pthread_mutex_unlock(&(m_q->mutex));

    return true;    
}

task_t* async_cond_queue::front(int milliseconds)
{
    if(!m_q || !m_q->task_queue)
    {
        return NULL;
    }
    
    task_t *t = NULL;
    struct timespec now;
    struct timespec timeout;
    pthread_mutex_lock(&(m_q->mutex));
    if(empty())
    {
        m_q->wait_threads_count++;
        while(empty() && m_q->quit == 0)
        {
            clock_gettime(CLOCK_REALTIME, &now);
            timeout.tv_sec = now.tv_sec + milliseconds / 1000;
            timeout.tv_nsec = now.tv_nsec + (milliseconds % 1000) * 1000 * 1000;
            
            pthread_cond_timedwait(&(m_q->cond), &(m_q->mutex), &timeout);
        }

        m_q->wait_threads_count--;
    }

    t = m_q->task_queue->front();
    pthread_mutex_unlock(&(m_q->mutex));

    return t;
}

task_t* async_cond_queue::back()
{
    if(!m_q || !m_q->task_queue)
    {
        return NULL;
    }

    pthread_mutex_lock(&(m_q->mutex));
    task_t *t = m_q->task_queue->back();
    pthread_mutex_unlock(&(m_q->mutex));

    return t;
}

void async_cond_queue::pop()
{
    if(!m_q || !m_q->task_queue)
    {
        return;
    }

    pthread_mutex_lock(&(m_q->mutex));
    m_q->task_queue->pop();
    pthread_mutex_unlock(&(m_q->mutex));
}

bool async_cond_queue::push(const task_t *t)
{
    if(!t || !m_q || !m_q->task_queue || m_q->quit == 1)
    {
        return false;
    }

    pthread_mutex_lock(&(m_q->mutex));
    m_q->task_queue->push(t);
    if(m_q->wait_threads_count > 0)
    {
        pthread_cond_signal(&(m_q->cond));
    }

    pthread_mutex_unlock(&(m_q->mutex));

    return true;    
}

bool async_cond_queue::full()
{
    if(!m_q || !m_q->task_queue)
    {
        return false;
    }

    return m_q->task_queue->full();
}

bool async_cond_queue::empty()
{
    if(!m_q || !m_q->task_queue)
    {
        return false;
    }

    return m_q->task_queue->empty();
}

int async_cond_queue::size()
{
    if(!m_q || !m_q->task_queue)
    {
        return 0;
    }

    return m_q->task_queue->size();
}

int async_cond_queue::capacity()
{
    if(!m_q || !m_q->task_queue)
    {
        return 0;
    }

    return m_q->task_queue->capacity();
}

void async_cond_queue::free()
{
    if(m_q)
    {
        if(m_q->task_queue)
        {
            delete m_q->task_queue;
            m_q->task_queue = NULL;
        }
        
        pthread_mutex_destroy(&(m_q->mutex));
        pthread_cond_destroy(&(m_q->cond));        

        delete m_q;
        m_q = NULL;
    }
}

