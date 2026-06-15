// BlockingQueue 实现

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class BlockingQueue {
private:
    size_t capacity_;
    bool stopped_;
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable cv_not_empty_;
    std::condition_variable cv_not_full_;
public:
    explicit BlockingQueue(size_t capacity): capacity_(capacity) {};
    ~BlockingQueue();

    bool push(const T& value) {

    }
    bool push(const T&& value) {}

    bool pop(T& out) {}
    
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stopped_ = true;
        }
        // 通知所有阻塞线程
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mtx_); 
        return queue_.empty();}

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.size();}
};
