#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <vector>
#include <functional>
#include <condition_variable>
#include <mutex>

class ThreadPool{
public:
    using Job = std::function<void(void)>; // alias.

    ThreadPool();
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete; //no copy calls
    ThreadPool& operator=(const ThreadPool&) = delete; // no assignement copy calls

    int	createTask();
    bool postTask(int task) const;
    void performTask(int task, const Job& job);
    void taskWait(int task);

private:
    void worker_main();

    bool running;
    mutable std::mutex mutex_t;
    std::vector<std::thread> threads_t;

    size_t taskSize;
    std::vector<int> freeTasks;
    std::vector<unsigned> pendingTasksCount;

    size_t jobSize;
    std::vector<int> jobTasks;
    std::vector<Job> jobFunctions;

    /* Condition variables */
    std::condition_variable worker_cv;
    std::condition_variable master_cv;
};

#endif