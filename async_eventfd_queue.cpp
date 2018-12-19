#include <iostream>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "async_eventfd_queue.h"

#define MAX_EVENTS 1024

async_eventfd_queue::async_eventfd_queue() :
    m_created(false),
    m_q(NULL)
{

}

async_eventfd_queue::~async_eventfd_queue()
{
    free();
}

bool async_eventfd_queue::create(const int capacity)
{
    if(m_created)
    {
        return true;
    }

    m_q = new eventfd_queue_t;
    if(!m_q)
    {
        std::cout << "create async eventfd queue failed." << std::endl;
        return false;
    }

    m_q->task_queue = new queue();
    if(!m_q->task_queue || !m_q->task_queue->create(capacity))
    {
        free();
        std::cout << "create task queue failed." << std::endl;
        return false;
    }

    if((m_q->epfd = epoll_create(1)) == -1)
    {
        free();
        std::cout << "create epoll failed." << std::endl;;
        return false;
    }
#if 0
    if((m_q->evfd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)) == -1)
    {
        free();
        return false;
    }
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = m_q->evfd;
    
    if(epoll_ctl(m_q->epfd, EPOLL_CTL_ADD, m_q->evfd, &ev) != 0)
    {
        free();
        return false;
    }
#endif

    m_q->quit = 0;
    m_created = true;    

    return true;
}

bool async_eventfd_queue::destroy()
{

}

task_t* async_eventfd_queue::front(int milliseconds)
{
    if(!m_q || !m_q->task_queue || m_q->epfd < 0)
    {
        return NULL;
    }

    std::cout << "before front queue size: " << size() << ", thread id: " << pthread_self() << std::endl;

    if(empty())
    {
        int count = 0;
        struct epoll_event events[MAX_EVENTS];
        if((count = epoll_wait(m_q->epfd, events, MAX_EVENTS, -1)) <= 0)
        {
            std::cout << "epoll_wait failed, error: " << count << ", thread_id: " << pthread_self() << std::endl;
            return NULL;
        }

        long long i = 0;
        read(events[0].data.fd, &i, sizeof(i));

#if 1
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = events[0].data.fd;
        
        epoll_ctl(m_q->epfd, EPOLL_CTL_DEL, events[0].data.fd, &ev);
        close(events[0].data.fd);
#endif
    }

    //std::cout << "end front queue size: " << size() << std::endl;

    return m_q->task_queue->front();
}

task_t* async_eventfd_queue::back()
{
    if(!m_q || !m_q->task_queue || empty())
    {
        return NULL;
    }

    return m_q->task_queue->back();
}

void async_eventfd_queue::pop()
{
    if(!m_q || !m_q->task_queue || empty())
    {
        return;
    }

    m_q->task_queue->pop();
}

bool async_eventfd_queue::push(const task_t *t)
{
    if(!t || !m_q || !m_q->task_queue)
    {
        return false;
    }

    if(!m_q->task_queue->push(t))
    {
        std::cout << "push task failed." << std::endl;
        return false;
    }

#if 1
    int evfd = -1;
    if((evfd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)) == -1)
    {
        std::cout << "eventfd failed.\n";
        free();
        return false;
    }
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = evfd;
    
    if(epoll_ctl(m_q->epfd, EPOLL_CTL_ADD, evfd, &ev) != 0)
    {
        free();
        std::cout << "epoll ctl add failed\n";
        return false;
    }
#endif

    long long i = 1;
    int ret = write(evfd, &i, sizeof(i));
    
    return true;
}

bool async_eventfd_queue::full()
{
    if(!m_q || !m_q->task_queue)
    {
        return false;
    }

    return m_q->task_queue->full();
}

bool async_eventfd_queue::empty()
{
    if(!m_q || !m_q->task_queue)
    {
        return false;
    }
    
    return m_q->task_queue->empty();
}

int async_eventfd_queue::size()
{
    if(!m_q || !m_q->task_queue)
    {
        return 0;
    }

    return m_q->task_queue->size();
}

int async_eventfd_queue::capacity()
{
    if(!m_q || !m_q->task_queue)
    {
        return 0;
    }

    return m_q->task_queue->capacity();
}

void async_eventfd_queue::free()
{
    if(m_q)
    {
        if(m_q->task_queue)
        {
            delete m_q->task_queue;
            m_q->task_queue = NULL;
        }

        if(m_q->evfd >= 0)
        {
            close(m_q->evfd);
        }
        
        if(m_q->epfd >= 0)
        {
            close(m_q->epfd);
        }

        delete m_q;
        m_q = NULL;
    }
}

