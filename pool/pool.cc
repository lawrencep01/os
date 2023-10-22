#include "pool.h"

using namespace std;

Task::Task() {
}

Task::~Task() {
}

void ThreadPool::Run(){
    while (true){
        pthread_mutex_lock(&stop_lock);
        if (stop) {
            pthread_mutex_unlock(&stop_lock);
            return;
        }
        pthread_mutex_unlock(&stop_lock);

        pthread_mutex_lock(&queue_lock);
        if (tasks.empty()) {
            pthread_cond_wait(&ready, &queue_lock); 
        }
        if (tasks.empty()) {  
            pthread_mutex_unlock(&queue_lock);
            continue; 
        }
        Task *task = tasks.front();
        tasks.pop();
        pthread_mutex_unlock(&queue_lock);

        task->Run();
        pthread_mutex_lock(&(task->done_lock));
        task->done = true;
        pthread_cond_broadcast(&(task->done_cond));
        pthread_mutex_unlock(&(task->done_lock));
    }
}

void *Helper(void *instance) {
    ThreadPool* tptr = (ThreadPool*)instance;
    tptr->Run();
    return NULL;
}

ThreadPool::ThreadPool(int num_threads) {
    // initialize muxes & cond inits
    pthread_mutex_init(&queue_lock, NULL);
    pthread_mutex_init(&map_lock, NULL);
    pthread_mutex_init(&stop_lock, NULL);
    pthread_cond_init(&ready, NULL);

    pthread_mutex_lock(&stop_lock);
    stop = false;
    pthread_mutex_unlock(&stop_lock);

    for (int i = 0; i < num_threads; i ++){
        pthread_t thread;
        pthread_create(&thread, NULL, Helper, (void*)this);
        threads.push_back(thread);
    }
}

void ThreadPool::SubmitTask(const std::string &name, Task* task) {

    pthread_mutex_lock(&(queue_lock));
    pthread_mutex_lock(&(map_lock));

    // mutex and conditional var on done variable of task
    pthread_mutex_init(&(task->done_lock), NULL);
    pthread_cond_init(&(task->done_cond), NULL);

    pthread_mutex_lock(&(task->done_lock));
    task->done = false;
    pthread_mutex_unlock(&(task->done_lock));

    nameToTask.insert({name, task});
    tasks.push(task);

    pthread_mutex_unlock(&(map_lock));
    pthread_mutex_unlock(&(queue_lock));
    pthread_cond_broadcast(&ready);
}

void ThreadPool::WaitForTask(const std::string &name) {
    pthread_mutex_lock(&map_lock);
    Task *task = nameToTask.at(name);
    pthread_mutex_unlock(&map_lock);

    pthread_mutex_lock(&(task->done_lock));
    while (!task->done){
        pthread_cond_wait(&(task->done_cond), &(task->done_lock));
    }
    pthread_mutex_unlock(&(task->done_lock));

    pthread_mutex_lock(&map_lock);
    nameToTask.erase(name);
    pthread_mutex_unlock(&map_lock);
    pthread_mutex_destroy(&(task->done_lock));
    delete task;
}

void ThreadPool::Stop() {
    pthread_mutex_lock(&stop_lock);
    stop = true;
    pthread_mutex_unlock(&stop_lock);

    pthread_cond_broadcast(&ready);
    for (pthread_t thread : threads){
        pthread_join(thread, NULL);
    }
    pthread_mutex_destroy(&stop_lock);
    pthread_mutex_destroy(&map_lock);
    pthread_mutex_destroy(&queue_lock);
}
