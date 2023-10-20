#ifndef POOL_H_
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <unordered_map>
using namespace std;

class Task {
public:
    bool done; // indicates whether task has been completed
    pthread_cond_t done_condition; // condition variable for task completion
    pthread_mutex_t done_lock; // mutex lock to synchronize access

    Task();
    virtual ~Task();
    virtual void Run() = 0;  // implemented by subclass
};

class ThreadPool {
public:
    unordered_map<string, Task*> tasks;  // hold tasks IDs
    queue<Task*> tasks_queue;  
    vector<pthread_t> threads;  // hold thread IDs
    pthread_mutex_t lock;  // mutex for locking tasks_queue
    pthread_cond_t is_ready;  // signal when a task is ready
    bool stop;

    ThreadPool(int num_threads);

    void RunThread();

    // Submit a task with a particular name.
    void SubmitTask(const std::string &name, Task *task);
 
    // Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
    void WaitForTask(const std::string &name);

    // Stop all threads. All tasks must have been waited for before calling this.
    // You may assume that SubmitTask() is not caled after this is called.
    void Stop();
};

#endif
