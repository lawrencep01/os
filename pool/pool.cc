#include "pool.h"

using namespace std;

Task::Task() {
}

Task::~Task() {
}

void ThreadPool::RunThread() {
    while (true) {
        pthread_mutex_lock(&lock);
        if (stop) {  
            pthread_mutex_unlock(&lock);
            return;
        }
        if (tasks_queue.empty()) {
            pthread_cond_wait(&is_ready, &lock); 
        }
        if (tasks_queue.empty()) {  
            pthread_mutex_unlock(&lock);
            continue; 
        }
        Task *task = tasks_queue.front();
        tasks_queue.pop();
        pthread_mutex_unlock(&lock);
        task->Run();
        pthread_mutex_lock(&(task->done_lock));
        task->done = true;
        pthread_cond_broadcast(&(task->done_condition));
        pthread_mutex_unlock(&(task->done_lock));
    }
}

void *Helper(void *instance) {
    ThreadPool* tptr = (ThreadPool*)instance;
    tptr->RunThread();
    return NULL;
}

// constructor, creates specified number of threads to run queued tasks
ThreadPool::ThreadPool(int num_threads) {
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_lock(&lock);
    stop = false;
    pthread_cond_init(&is_ready, NULL);
    for (int i = 0; i < num_threads; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, Helper, (void*)this);
        threads.push_back(thread);
    }
    pthread_mutex_unlock(&lock);
}


// enqueue a specified task by "name" identifier
void ThreadPool::SubmitTask(const std::string &name, Task* task) {
    pthread_mutex_lock(&lock);  
    pthread_cond_init(&(task->done_condition), NULL);
    pthread_mutex_init(&(task->done_lock), NULL);
    
    task->done = false;  
    
    tasks.insert({name, task});
    tasks_queue.push(task);
    pthread_cond_broadcast(&is_ready);
    pthread_mutex_unlock(&lock);
}

// wait for a particular task to complete
void ThreadPool::WaitForTask(const std::string &name) {
    pthread_mutex_lock(&lock);
    Task *task = tasks.at(name);
    pthread_mutex_unlock(&lock);

    pthread_mutex_lock(&task->done_lock);
    while (!task->done) {
        pthread_cond_wait(&(task->done_condition), &(task->done_lock));
    }
    pthread_mutex_unlock(&task->done_lock);

    pthread_mutex_lock(&lock);
    tasks.erase(name);
    pthread_mutex_unlock(&lock); 

    pthread_mutex_destroy(&(task->done_lock));
    delete task;
}

// stop the thread pool; terminate normally
void ThreadPool::Stop() {
    pthread_mutex_lock(&lock);
    stop = true;
    pthread_cond_broadcast(&is_ready);
    pthread_mutex_unlock(&lock);
    for (pthread_t thread : threads) {
        pthread_join(thread, NULL);
    }
    pthread_mutex_destroy(&lock);
}
