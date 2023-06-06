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
    ThreadPool(size_t); // ��ʼ���̳߳�
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
    size_t GetActiveWorkerCount();

    ~ThreadPool();

private:
    std::vector<std::thread> workers;        // �����߳�
    std::queue<std::function<void()>> tasks; // �������
    size_t activeWorkers;                    // ��Ծ�߳���

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false), activeWorkers(0)
{
    // ����threads���߳�
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this] { // �߳�ʵ�� ��ѭ��
                for (;;)
                {
                    std::function<void()> task; // ��ִ������
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        // �ȴ�stop��Ϊtrue����������в�Ϊ��
                        this->condition.wait(lock,
                                             [this]
                                             {
                                                 return this->stop || !this->tasks.empty();
                                             });
                        // �̳߳�ֹͣ��������Ϊ�գ����߳̽���
                        if (this->stop && this->tasks.empty())
                            return;
                        // ȡ��һ������ִ��
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                        ++activeWorkers;
                        DFLOG_DEBUG("����ʼ, ��������:%d ʣ���������:%d", GetActiveWorkerCount(), tasks.size());
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        --activeWorkers; // ����ִ����ϣ����ټ���
                        DFLOG_DEBUG("����������������� %d ʣ���������:%d", GetActiveWorkerCount(), tasks.size());
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
            future.get(); // �ȴ��������
    }
};

#endif
