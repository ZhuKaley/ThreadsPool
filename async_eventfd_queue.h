#ifndef __ASYNC_EVENTFD_QUEUE_H__
#define __ASYNC_EVENTFD_QUEUE_H__

#include "queue.h"
#include "base_async_queue.h"


typedef struct eventfd_queue
{
    queue *task_queue;
    int epfd;   //epoll fd
    int evfd;   //event fd
    int quit;   //0: thread not quit,  1: thread quit

    eventfd_queue() :
        epfd(-1),
        quit(0)/*,
        wait_threads_count(0)*/
    {

    }
} eventfd_queue_t;

class async_eventfd_queue : public base_async_queue
{
public:
    async_eventfd_queue();
    ~async_eventfd_queue();

    bool create(const int capacity);
    bool destroy();
    
    task_t* front(int milliseconds = 0);
    task_t* back();
    
    void pop();
    bool push(const task_t *t);
    
    bool full();
    bool empty();

    int size();
    int capacity();

private:
    void free();

private:
    bool m_created;
    eventfd_queue_t *m_q;
        
};

#endif
