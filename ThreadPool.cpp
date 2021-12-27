//
// Created by miraclemilk on 2021/12/27.
//

#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(int workNum)
    :stop(false)
{
    workNum = workNum <= 0 ? 1: workNum;
    for (int i = 0; i < workNum; ++i)
    {
        this->threads.emplace_back([this](){
            while(1)
            {
                JobFunction func;
                {
                    std::unique_lock<std::mutex> lock(this->lock);
                    while (!this->stop && this->jobs.empty())
                    {
                        this->conditon.wait(lock);
                    }
                    if(this->jobs.empty() && this->stop)
                    {
                        return;
                    }
                    func = this->jobs.front();
                    this->jobs.pop();
                }
                if(func)
                {
                    func();
                }
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex>lock(this->lock);
        this->stop = true;
    }
    this->conditon.notify_all();
    for(auto &thread:this->threads)
    {
        thread.join();
    }
}

void ThreadPool::pushJob(const JobFunction &job)
{
    {
        std::unique_lock<std::mutex>lock(this->lock);
        this->jobs.push(job);
    }
    this->conditon.notify_one();
}