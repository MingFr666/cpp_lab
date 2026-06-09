#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>


/*
std::function 是 C++ 标准库里的一个函数包装器。
它可以存储、复制和调用任何可调用对象（函数、lambda 表达式、函数指针、成员函数指针等）。

*/
class ThreadPool {
public:
    explicit ThreadPool(size_t thread_count)
        : stopped_(false)
    {
        if (thread_count == 0) {
            throw std::invalid_argument("thread_count must be greater than 0");
        }

        for (size_t i = 0; i < thread_count; ++i) {
            workers_.emplace_back([this] {
                worker_loop();
            });
        }
    }

    ~ThreadPool() {
        stop();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <typename Func, typename... Args>
    auto submit(Func&& func, Args&&... args)
        -> std::future<decltype(func(args...))>
    {
        using ReturnType = decltype(func(args...));

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
        );

        std::future<ReturnType> result = task->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (stopped_) {
                throw std::runtime_error("submit on stopped ThreadPool");
            }

            tasks_.emplace([task] {
                (*task)();
            });
        }

        cv_.notify_one();

        return result;
    }

private:
    void worker_loop() {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(mutex_);

                cv_.wait(lock, [this] {
                    return stopped_ || !tasks_.empty();
                });

                if (stopped_ && tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
            }

            task();
        }
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (stopped_) {
                return;
            }

            stopped_ = true;
        }

        cv_.notify_all();

        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex mutex_;
    std::condition_variable cv_;

    bool stopped_;
};

int add(int a, int b) {
    return a + b;
}

int main() {
    ThreadPool pool(4);

    auto f1 = pool.submit(add, 1, 2);

    auto f2 = pool.submit([] {
        std::cout << "hello from thread pool" << std::endl;
    });

    auto f3 = pool.submit([](int x) {
        return x * x;
    }, 10);

    std::cout << "f1 result = " << f1.get() << std::endl;
    f2.get();
    std::cout << "f3 result = " << f3.get() << std::endl;

    return 0;
}