#ifndef __QUEUE_H__
#define __QUEUE_H__

#define DEFAULT_QUEUE_CAPACITY 65535

typedef struct task
{
    void* (*run)(void *);
    void *arg;
} task_t;

typedef struct task_queue
{
    int head;
    int tail;
    int size;
    int capacity;
    task_t *tasks;

    task_queue() :
        head(0),
        tail(0),
        size(0),
        capacity(0)
    {

    }
} task_queue_t;

class queue
{
public:
    queue();
    ~queue();

    bool create(int capacity);
    task_t* front();
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
    task_queue_t *m_q;
};


#endif
