#include <thread_pool.h>

ThreadPool::ThreadPool(): running(true), taskSize(0u), jobSize(0u){
    size_t thread_count;
    thread_count = std::thread::hardware_concurrency();
    if (thread_count == 0)
        thread_count = 1;
    threads_t.reserve(thread_count);

    for (size_t i = 0; i < thread_count; ++i)
        threads_t.emplace_back(&ThreadPool::worker_main, this);
}

ThreadPool::~ThreadPool(){
    std::unique_lock<std::mutex> lock(mutex_t);
    running = false;
    worker_cv.notify_all();
    lock.unlock();
    for (auto& t : threads_t){
        t.join();
    }
}

int ThreadPool::task_create(){
    std::lock_guard<std::mutex> lock(mutex_t);
    int task;
    if (freeTasks.empty()){
        task = static_cast<int>(taskSize);
        taskSize++;
        pendingTasksCount.resize(taskSize);
    }
    else{
        task = freeTasks.back();
        freeTasks.pop_back();
    }
    pendingTasksCount[task] = 0u;
    return task;
}

bool ThreadPool::task_finished(int task) const{
    std::lock_guard<std::mutex> lock(mutex_t);
    return pendingTasksCount[task] == 0u;
}

void ThreadPool::task_perform(int task, const Job& job){
    std::lock_guard<std::mutex> lock(mutex_t);
    jobTasks.push_back(task);
    jobFunctions.push_back(job);
    pendingTasksCount[task]++;
    jobSize++;
    worker_cv.notify_one();
}

void ThreadPool::task_wait(int task){
    std::unique_lock<std::mutex> lock(mutex_t);

    while (pendingTasksCount[task] > 0)
        master_cv.wait(lock);

    freeTasks.push_back(task);
}

void ThreadPool::worker_main(){
    std::unique_lock<std::mutex> lock(mutex_t);
    while (running){
        worker_cv.wait(lock);
        for (;;){
            if (jobSize == 0)
                break;
            int task = jobTasks.back();
            Job job = jobFunctions.back();
            jobTasks.pop_back();
            jobFunctions.pop_back();
            jobSize--;
            lock.unlock();
            job();
            lock.lock();
            pendingTasksCount[task]--;
            if (pendingTasksCount[task] == 0){
                master_cv.notify_all();
            }
        }
    }
}