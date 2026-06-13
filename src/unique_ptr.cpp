// unique_ptr 智能指针实现
/*
    UniquePtr 简化实现

    构造 / 析构
    禁止拷贝
    支持移动
    operator*
    operator->
    get
    reset
    release
    自定义 deleter 可选
*/


// unique_ptr 是一个独占所有权的智能指针，确保资源在不再需要时被正确释放
// 默认 deleter 调用 delete ptr;
// delete 会做两件事：
// 1. 调用对象的析构函数
// 2. 释放这块对象内存

// 但自定义 deleter 的原因，不是简单因为“有些没有析构函数”，而是因为：
// 资源的释放方式不是 delete，可能需要调用特定的函数来释放资源，比如 FILE* 需要调用 fclose()，而不是 delete。

#include <iostream>
#include <memory> // 包含 std::unique_ptr 的定义
#include <utility> // 包含 std::move 的定义

template<typename T>
class UniquePtr
{
private:
    T* ptr_;
public:
    explicit UniquePtr(T* ptr = nullptr): ptr_(ptr) {};
    ~UniquePtr() {
        delete ptr_;
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept 
    : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T& operator*() {
        return *ptr_;
    }

    T* operator->() {
        return ptr_;
    }

    T* get() {
        return ptr_;
    }

    T* release() {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* ptr = nullptr) {
        if (ptr_ != ptr) {
            delete ptr_;
            ptr_ = ptr;
        }
    }
};


int main() {
    UniquePtr<int> up1(new int(42));
    std::cout << *up1 << std::endl; // 输出 42

    UniquePtr<int> up2 = std::move(up1); // 移动构造
    std::cout << *up2 << std::endl; // 输出 42

    up1.reset(new int(100)); // 重置 up1，释放原资源并接管新资源
    std::cout << *up1 << std::endl; // 输出 100

    auto rawPtr = up1.release(); // 释放 up1 的所有权，返回原指针
    std::cout << *rawPtr << std::endl; // 输出 100
    delete rawPtr; // 手动释放资源

    // 标准库的unique_ptr使用方法

    std::unique_ptr<int> stdUp1(new int(55));
    std::cout << *stdUp1 << std::endl; // 输出 55   
    std::unique_ptr<int> stdUp2 = std::make_unique<int>(99); // 使用 make_unique 创建 unique_ptr

    // make_unique 是 C++14 引入的一个工厂函数，用于创建 unique_ptr，提供了更安全和简洁的语法，避免了直接使用 new 可能带来的内存泄漏风险。

    return 0;
}
