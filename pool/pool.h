#ifndef POOL_H_
#include <string>
#include <pthread.h>
#include <queue>
#include <map>
#include <iostream>

using namespace std;

class Task {
public:
    bool done;
    pthread_mutex_t done_lock;
    pthread_cond_t done_cond;
    Task();
    virtual ~Task();
    virtual void Run() = 0;  // implemented by subclass
};

class ThreadPool {
public:
    bool stop;
    map<string, Task*> nameToTask;
    queue<Task*> tasks;
    vector<pthread_t> threads;
    pthread_mutex_t queue_lock;
    pthread_mutex_t map_lock;
    pthread_cond_t ready;
    pthread_mutex_t stop_lock;

    ThreadPool(int num_threads);

    // Run the thread
    void Run();

    // Submit a task with a particular name.
    void SubmitTask(const std::string &name, Task *task);
 
    // Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
    void WaitForTask(const std::string &name);

    // Stop all threads. All tasks must have been waited for before calling this.
    // You may assume that SubmitTask() is not caled after this is called.
    void Stop();
};
#endif
