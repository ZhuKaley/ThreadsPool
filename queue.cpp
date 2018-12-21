#include <iostream>

#include "queue.h"

queue::queue() :
    m_q(NULL)
{

}

queue::~queue()
{
    free();
}

bool queue::create(int capacity)
{
    if(!m_q)
    {
        free();
    }

    m_q = new task_queue_t;
    if(!m_q)
    {
        return false;
    }

    if(capacity > 0)
    {
        m_q->tasks = new task_t[capacity];
        m_q->capacity = capacity;
    }
    else
    {
        int default_capacity = DEFAULT_QUEUE_CAPACITY;
        m_q->tasks = new task_t[default_capacity];
        m_q->capacity = default_capacity;
    }

    if(!m_q->tasks)
    {
        free();
        return false;
    }

    m_q->head = 0;
    m_q->tail = 0;
    m_q->size = 0;

    return true;
}

task_t* queue::front()
{
    if(empty())
    {
        return NULL;
    }

    if(m_q->head >= m_q->capacity || m_q->head < 0)
    {        
        std::cout << "task index: " << m_q->head << std::endl;
    }

    return &(m_q->tasks[m_q->head]);
}

task_t* queue::back()
{
    if(empty())
    {
        return NULL;
    }

    return &m_q->tasks[(m_q->tail - 1 + m_q->capacity) % m_q->capacity];
}

void queue::pop()
{
    if(empty())
    {
        return;
    }

    if(m_q->head >= m_q->capacity || m_q->head < 0)
    {
        std::cout << "pop head: " << m_q->head << std::endl;
    }

    m_q->head = (m_q->head + 1) % m_q->capacity;
    m_q->size--;

    //std::cout << "m_q->head: " << m_q->head << std::endl;
}

bool queue::push(const task_t *t)
{
    if(!t || full())
    {
        return false;
    }

    if(m_q->tail > m_q->capacity || m_q < 0)
    {
        std::cout << "push failed." << std::endl;
    }

    m_q->tasks[m_q->tail].run = t->run;
    m_q->tasks[m_q->tail].arg = t->arg;

    m_q->tail = (m_q->tail + 1) % m_q->capacity;
    m_q->size++;

    return true;
}

bool queue::full()
{
    if(!m_q || !m_q->tasks)
    {
        return false;
    }

    return m_q->size == m_q->capacity;
}

bool queue::empty()
{
    if(!m_q || !m_q->tasks)
    {
        return false;
    }

    return m_q->size == 0;
}

int queue::size()
{
    if(!m_q)
    {
        return 0;
    }

    return m_q->size;
}

int queue::capacity()
{
    if(!m_q)
    {
        return 0;
    }

    return m_q->capacity;
}

void queue::free()
{
    if(m_q)
    {
        if(m_q->tasks)
        {
            delete[] m_q->tasks;
            m_q->tasks = NULL;
        }
        
        delete m_q;
        m_q = NULL;
    }
}



