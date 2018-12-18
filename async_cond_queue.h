#ifndef __ASYNC_COND_QUEUE_H__
#define __ASYNC_COND_QUEUE_H__

#include <pthread.h>

#include "queue.h"

typedef struct cond_queue
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    queue *task_queue;
    int quit;   //0: thread not quit,  1: thread quit
    int wait_threads_count;

    cond_queue() :
        quit(0),
        wait_threads_count(0)
    {

    }
} cond_queue_t;

class async_cond_queue
{
public:
    async_cond_queue();
    ~async_cond_queue();

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
    cond_queue_t *m_q;
};

#endif
