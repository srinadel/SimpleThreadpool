# SimpleThreadpool
Simple Thread pool in C++ 11.

Each Task contains jobs. Main thread can wait on each task(till all the jobs in the task are done).
Uses mutex and condition variables.

# Sample Use case:
create task. Add multiple jobs to a task. Task completion is dependant on the completion of it's jobs.
Each thread in the threadpool runs the available jobs. Upon completion of each job, it is marked as done and the count of pending jobs in the task is reduced.

#TODO:
1.) Add support for async.