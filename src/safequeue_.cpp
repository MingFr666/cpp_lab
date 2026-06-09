// 线程安全队列

// C++ 里的 queue 通常指 std::queue，它本身不是一个真正的容器，而是一个容器适配器 container adaptor。
// deque 的底层实现是一个双端队列，提供了高效的插入和删除操作，适合用作队列的底层容器。 分段连续内存 + 中控数组 / map 管理这些内存块


/* 
* lock_guard 和 unique_lock 都是 C++ 标准库提供的 RAII 风格的锁管理类，用于简化互斥锁的使用，确保在作用域结束时自动释放锁。
* 两者的区别
* 1. lock_guard 是一个简单的封装类，提供了基本的锁管理功能。它在构造时获取锁，在析构时释放锁。lock_guard 不支持手动解锁或重新锁定，因此适用于简单的场景。
* 2. unique_lock 提供了更灵活的锁管理功能，支持手动解锁、重新锁定以及延迟锁定，适用于复杂的场景。
*   - unique_lock 可以在构造时选择是否立即获取锁，或者在需要时再获取锁。需要同时锁多个 mutex 时有用
*   - unique_lock 还提供了 try_lock() 方法，可以尝试获取锁而不阻塞线程，如果锁已经被其他线程持有，则返回 false。
*   - unique_lock 还支持条件变量的等待和通知，可以与 std::condition_variable 一起使用，提供更强大的线程同步功能。


* condition_variable 是 C++ 标准库提供的一个同步原语，用于在线程之间进行通知和等待。它允许一个或多个线程等待某个条件的发生，并在条件满足时被唤醒。
* condition_variable 通常与 std::mutex 一起使用，以确保对共享资源的访问是线程安全的。

* std::optional 是 C++17 引入的一个模板类，用于表示一个可能存在或不存在的值。它提供了一种安全的方式来处理可能为空的值，避免了使用裸指针或特殊值来表示缺失数据。

* 虚假唤醒 spurious wakeup 是指线程在等待条件变量时被唤醒，但实际上条件并没有满足。这可能是由于操作系统的调度机制或其他因素引起的。
* 为了处理虚假唤醒，通常在等待条件变量时使用一个循环来检查条件是否满足，而不是直接假设被唤醒就意味着条件已经满足。

显式 delete move 构造函数和移动赋值运算符，禁止移动语义
如果一个类定义了移动构造函数或移动赋值运算符，但不希望允许移动语义，可以将这些函数显式删除（delete）。这样子move后，连拷贝构造函数和拷贝赋值运算符都无法使用了，整个类就变成了不可复制和不可移动的类型。
*/

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <thread>
#include <iostream>

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;

    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void push(const T& value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(value);
        }
        cv_.notify_one();
    }

    void push(T&& value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(value));
        }
        cv_.notify_one();
    }

    /*
    * wait_pop 方法用于从队列中弹出一个元素，如果队列为空，则等待直到有元素可用或队列被停止。
    * bool + T& 版本的 wait_pop 方法返回一个布尔值，表示是否成功弹出一个元素。如果队列被停止且为空，则返回 false；否则返回 true，并通过引用参数 value 返回弹出的元素。

    worker 线程尝试锁住 mtx
    如果 mtx 没被别人锁住，worker 立刻获得 mtx
    如果 mtx 已经被别人锁住，worker 阻塞，直到拿到 mtx

    CV内部会先检查条件,如果条件不满足，才会阻塞当前线程并释放 mutex 以允许其他线程修改条件
    等待其他线程 notify_one / notify_all
    等待的线程需要再次检查条件是否满足，因为可能存在虚假唤醒的情况，即线程被唤醒但条件实际上并未满足。因此，通常在使用 CV 时会将等待操作放在一个循环中，以确保条件真正满足后才继续执行。

    */
    bool wait_pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_.wait(lock, [this] {
            return stopped_ || !queue_.empty();
        });

        if (stopped_ && queue_.empty()) {
            return false;
        }

        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (queue_.empty()) {
            return std::nullopt;
        }

        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopped_ = true;
        }
        cv_.notify_all();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<T> queue_;
    bool stopped_ = false;
};

// 使用示例
int main() {
    ThreadSafeQueue<int> tsq;

    // 生产者线程
    std::thread producer([&tsq] {
        for (int i = 0; i < 10; ++i) {
            tsq.push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟生产时间
        }
        tsq.stop(); // 生产完成后停止队列
    });

    // 消费者线程
    std::thread consumer([&tsq] {
        int value;
        while (tsq.wait_pop(value)) {
            std::cout << "Consumed1: " << value << std::endl;
            // std::this_thread::sleep_for(std::chrono::milliseconds(150)); // 模拟消费时间
        }
    });

    std::thread consumer2([&tsq] {
        int value;
        while (tsq.wait_pop(value)) {
            std::cout << "Consumed2: " << value << std::endl;
            // std::this_thread::sleep_for(std::chrono::milliseconds(150)); // 模拟消费时间
        }
    });

    producer.join();
    consumer.join();
    consumer2.join();

    return 0;
}