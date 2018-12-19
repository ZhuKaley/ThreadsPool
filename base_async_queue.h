#ifndef __BASE_ASYNC_QUEUE_H__
#define __BASE_ASYNC_QUEUE_H__

#include "queue.h"

class base_async_queue
{
public:
    base_async_queue() {};
    virtual ~base_async_queue() {};

    virtual bool create(const int capacity) = 0;
    virtual bool destroy() = 0;
    
    virtual task_t* front(int milliseconds = 0) = 0;
    virtual task_t* back() = 0;
    
    virtual void pop() = 0;
    virtual bool push(const task_t *t) = 0;
    
    virtual bool full() = 0;
    virtual bool empty() = 0;

    virtual int size() = 0;
    virtual int capacity() = 0;

};

#endif
