#include <iostream>

#include "queue.h"
#include "threads_pool.h"

void* test(void *arg)
{
    //std::cout << "int test..." << std::endl;
    //std::cout << "arg: " << *(int *)arg << std::endl;

    std::cout << "current thread id: " << pthread_self() << std::endl;
}

int main()
{
    std::cout << "running..." << std::endl;
    
    threads_pool pool;
    if(!pool.create(5, 10))
    {
        std::cout << "create thread pool failed." << std::endl;
        return -1;
    }

    pool.add_task(test, NULL);
    while(1)
    {
        sleep(1);
    }
    
    pool.destroy();

    return 0;
}

int main1()
{
    queue q;
    if(!q.create(5))
    {
        std::cout << "cteate queue failed." << std::endl;
        return -1;
    }

    std::cout << "queue size: " << q.size() << std::endl;
    std::cout << "queue capacity: " << q.capacity() << std::endl;

    int i = 2;
    task_t t;
    t.run = test;
    t.arg = &i;
    i++;
    
    task_t t3;
    t3.run = test;
    t.arg = &i;
    i++;
    
    task_t t4;
    t4.run = test;
    t4.arg = &i;
    i++;
    
    task_t t5;
    t5.run = test;
    t5.arg = &i;

    int k = 6;
    task_t t6;
    t6.run = test;
    t6.arg = &k;

    q.push(&t);
    q.push(&t3);
    q.push(&t4);
    q.push(&t5);
    q.push(&t6);

    std::cout << "queue size: " << q.size() << std::endl;
    std::cout << "queue capacity: " << q.capacity() << std::endl;
    
    task_t *t1 = q.front();
    if(t1)
    {
        t1->run(t1->arg);
    }
    
    task_t *t2 = q.back();
    if(t2)
    {
        t2->run(t2->arg);
    }

    int size = q.size();
    for(int j = 0; j < size; j++)
    {
        q.pop();
        std::cout << "j: " << j << std::endl;
    }
    
    std::cout << "queue size: " << q.size() << std::endl;
    std::cout << "queue capacity: " << q.capacity() << std::endl;

    if(q.full())
    {
        std::cout << "queue full." << std::endl;
    }
    else
    {
        std::cout << "queue not full." << std::endl;
    }

    if(q.empty())
    {
        std::cout << "queue empty." << std::endl;
    }
    else
    {
        std::cout << "queue not empty." << std::endl;
    }

    if(!q.push(&t))
    {
        std::cout << "push failed." << std::endl;
    }
    else
    {
        std::cout << "push succed." << std::endl;
    }
    
    std::cout << "queue size: " << q.size() << std::endl;
    std::cout << "queue capacity: " << q.capacity() << std::endl;

    if(q.full())
    {
        std::cout << "queue full." << std::endl;
    }
    else
    {
        std::cout << "queue not full." << std::endl;
    }

    if(q.empty())
    {
        std::cout << "queue empty." << std::endl;
    }
    else
    {
        std::cout << "queue not empty." << std::endl;
    }
    
    return 0;
}
