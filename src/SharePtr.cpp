// share_ptr的实现，主要是通过引用计数来管理资源的生命周期
// 共享所有权的智能指针，允许多个指针实例共享同一个资源
// 当最后一个指针实例被销毁时，资源才会被释放

template<typename T>
class SharePtr {
private:
    T* ptr_;
    size_t* ref_count_; // 引用计数, 指向一个 size_t 类型的动态分配内存，记录有多少个 SharePtr 实例共享同一个资源

    void release() {
        if (ref_count_ && --(*ref_count_) == 0) {
            delete ptr_;
            delete ref_count_;
        }
    }
public:
    explicit SharePtr(T* ptr = nullptr) : ptr_(ptr), ref_count_(new size_t(1)) {}
    ~SharePtr() {
        release();
    }
    // 拷贝构造
    SharePtr(const SharePtr& other) : ptr_(other.ptr_), ref_count_(other.ref_count_) {
        ++(*ref_count_); // 增加引用计数
    }
    // 拷贝赋值
    SharePtr& operator=(const SharePtr& other) {
        if (this != &other) {
            release(); // 释放当前资源
            ptr_ = other.ptr_;
            ref_count_ = other.ref_count_;
            ++(*ref_count_); // 增加引用计数
        }
        return *this;
    }
    // 移动构造
    SharePtr(SharePtr&& other) noexcept : ptr_(other.ptr_), ref_count_(other.ref_count_) {
        other.ptr_ = nullptr;
        other.ref_count_ = nullptr;
    }
    // 移动赋值
    SharePtr& operator=(SharePtr&& other) noexcept {
        if (this != &other) {        
            release(); // 释放当前资源
            ptr_ = other.ptr_;
            ref_count_ = other.ref_count_;
            other.ptr_ = nullptr;
            other.ref_count_ = nullptr;
        }
        return *this;
    }

    T& operator*() const {
        return *ptr_;
    } 

    T* operator->() const {
        return ptr_;
    }

    T* get() const {
        return ptr_;
    }

    size_t use_count() const {
        return ref_count_ ? *ref_count_ : 0;
    }

};