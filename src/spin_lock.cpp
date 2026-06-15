// 自旋锁实现
// 线程拿不到锁时，不阻塞睡眠，而是在 CPU 上一直循环检查锁是否释放

#include <atomic>
#include <mutex>
#include <vector>
#include <thread>
#include <iostream>

class SpinLock {
private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // busy-wait
            // test_and_set() 会返回 flag 的旧值，然后把 flag 设置为 true，过程是原子的
            // atomic_flag 只有两个状态
            // 当前线程拿到锁之后，后面的读写不能被重排到 lock 之前。
        }
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
        // 把 atomic_flag 重新设置为 false
        // 当前线程在 unlock 之前的读写，不能被重排到 unlock 之后。
    }
};

SpinLock spinlock;
int counter = 0;

void add() {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<SpinLock> guard(spinlock);
        ++counter;
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; i++) {
        threads.emplace_back(add);
    }
    
    for (auto & t: threads) {
        t.join();
    }

    std::cout << counter << std::endl;

    return 0;
}