#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include "dflogger.h"

class ThreadPool
{
public:
    ThreadPool(size_t); // 初始化线程池
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
    size_t GetActiveWorkerCount();

    ~ThreadPool();

private:
    std::vector<std::thread> workers;        // 工作线程
    std::queue<std::function<void()>> tasks; // 任务队列
    size_t activeWorkers;                    // 活跃线程数

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false), activeWorkers(0)
{
    // 创建threads个线程
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this] { // 线程实现 死循环
                for (;;)
                {
                    std::function<void()> task; // 待执行任务
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        // 等待stop变为true或者任务队列不为空
                        this->condition.wait(lock,
                                             [this]
                                             {
                                                 return this->stop || !this->tasks.empty();
                                             });
                        // 线程池停止或者任务为空，则线程结束
                        if (this->stop && this->tasks.empty())
                            return;
                        // 取第一个任务，执行
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                        ++activeWorkers;
                        DFLOG_DEBUG("任务开始, 活动任务个数:%d 剩余任务个数:%d", GetActiveWorkerCount(), tasks.size());
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        --activeWorkers; // 任务执行完毕，减少计数
                        DFLOG_DEBUG("任务结束，活动任务个数 %d 剩余任务个数:%d", GetActiveWorkerCount(), tasks.size());
                    }
                }
            });
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]()
                      {
                          (*task)();
                      });
    }
    condition.notify_one();
    return res;
}

inline size_t ThreadPool::GetActiveWorkerCount()
{
    return activeWorkers;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
}

class TaskState
{
public:
    std::future<void> future;
    std::shared_ptr<std::atomic<bool>> shouldStop = std::make_shared<std::atomic<bool>>(false);

    void stop()
    {
        *shouldStop = true;
        if (future.valid())
            future.get(); // 等待任务结束
    }
};

#endif
