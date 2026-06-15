// share_ptr的实现，主要是通过引用计数来管理资源的生命周期
// 共享所有权的智能指针，允许多个指针实例共享同一个资源
// 当最后一个指针实例被销毁时，资源才会被释放
/*
    SharedPtr 简化实现
    控制块
    引用计数
    拷贝构造 / 拷贝赋值
    移动构造 / 移动赋值
    析构释放
    use_count
    reset
    多线程原子引用计数版本
*/
#include <atomic>
#include <iostream>

template<typename T>
class SharePtr
{
private:
    T* ptr_;
    std::atomic<int> *ref_count_;

    void release() {
        if (ref_count_) {
            if (ref_count_->fetch_sub(1) == 1) {
                delete ptr_;
                delete ref_count_;
            }
        }
        ptr_ = nullptr;
        ref_count_ = nullptr;
    }
public:
    explicit SharePtr(T* ptr=nullptr): ptr_(ptr), ref_count_(ptr_? new std::atomic<int>(1) : nullptr) {}
    ~SharePtr() {
        release();
    }

    SharePtr(const SharePtr& other) 
        : ptr_(other.ptr_), ref_count_(other.ref_count_) {
        if (ref_count_) {
            ref_count_->fetch_add(1);
        }
    }
    SharePtr& operator=(const SharePtr& other) {
        if (this != &other) {
            if (other.ref_count_) {
                other.ref_count_->fetch_add(1);
            }
            release();

            ptr_= other.ptr_;
            ref_count_ = other.ref_count_;
        }

        return *this;
    }

    SharePtr(SharePtr&& other) noexcept
        : ptr_(other.ptr_), ref_count_(other.ref_count_) {
        other.ptr_ = nullptr;
        other.ref_count_ =nullptr;
    }

    SharePtr& operator=(SharePtr&& other) noexcept {
        if (this != &other) {
            release();

            ptr_= other.ptr_;
            ref_count_ = other.ref_count_;
            other.ptr_ = nullptr;
            other.ref_count_ = nullptr;
        }

        return *this;
    }

    T& operator*() {
        return *ptr_;
    }

    T* operator->() {
        return ptr_;
    }

    void reset(T* ptr = nullptr) {
        if (ptr_ != ptr) {
            release();
            ptr_  = ptr;
            ref_count_ = ptr_ ? new std::atomic<int>(1) : nullptr;
        }
    }

    int use_count() const {
        return ref_count_ ? ref_count_->load() : 0;
    }
};



struct Foo {
    int x;

    Foo(int v) : x(v) {
        std::cout << "Foo constructed: " << x << std::endl;
    }

    ~Foo() {
        std::cout << "Foo destroyed: " << x << std::endl;
    }

    void hello() const {
        std::cout << "hello " << x << std::endl;
    }
};

int main() {
    SharePtr<Foo> p1(new Foo(10));

    std::cout << p1.use_count() << std::endl;  // 1

    SharePtr<Foo> p2 = p1;

    std::cout << p1.use_count() << std::endl;  // 2
    std::cout << p2.use_count() << std::endl;  // 2

    SharePtr<Foo> p3;
    p3 = p2;

    std::cout << p1.use_count() << std::endl;  // 3
    std::cout << p2.use_count() << std::endl;  // 3
    std::cout << p3.use_count() << std::endl;  // 3

    SharePtr<Foo> p4(std::move(p3));

    std::cout << p3.use_count() << std::endl;  // 0
    std::cout << p4.use_count() << std::endl;  // 3

    p4->hello();

    p4.reset(new Foo(20));

    std::cout << p1.use_count() << std::endl;  // 2
    std::cout << p4.use_count() << std::endl;  // 1

    return 0;
}
