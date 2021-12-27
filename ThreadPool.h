//
// Created by miraclemilk on 2021/12/27.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include <functional>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>

class ThreadPool {
public:
    using JobFunction = std::function<void()>;
    ThreadPool(int workNum);
    ~ThreadPool();
    void pushJob(const JobFunction &job);
private:
    std::vector<std::thread> threads;
    std::mutex lock;    //互斥锁
    std::condition_variable conditon;
    std::queue<JobFunction> jobs;
    bool stop;
};


#endif //WEBSERVER_THREADPOOL_H
