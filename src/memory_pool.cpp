// 可面试讲解的 C++ 内存池

#include <iostream>
#include <vector>
#include <new>
#include <cassert>

struct Order {
    int order_id;
    double price;
    int volume;

    Order(int id, double p, int v)
        : order_id(id), price(p), volume(v) {
        std::cout << "Order ctor: " << order_id << std::endl;
    }

    ~Order() {
        std::cout << "Order dtor: " << order_id << std::endl;
    }
};

template <typename T>
class ObjectPool {
private:
    union Slot {
        T object;
        Slot* next;

        Slot() {}
        ~Slot() {}
    };

private:
    Slot* memory_;        // 整块连续内存
    Slot* free_list_;     // 空闲链表头
    size_t capacity_;     // 池中对象槽位数量

public:
    explicit ObjectPool(size_t capacity)
        : memory_(nullptr),
          free_list_(nullptr),
          capacity_(capacity) {

        // 只分配原始内存，不调用 T 构造函数
        memory_ = static_cast<Slot*>(
            ::operator new[](sizeof(Slot) * capacity_)
        );

        // 初始化空闲链表
        for (size_t i = 0; i < capacity_ - 1; ++i) {
            memory_[i].next = &memory_[i + 1];
        }

        memory_[capacity_ - 1].next = nullptr;
        free_list_ = &memory_[0];
    }

    ~ObjectPool() {
        // 注意：这里假设所有对象都已经归还
        ::operator delete[](memory_);
    }

    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

public:
    template <typename... Args>
    T* create(Args&&... args) {
        if (free_list_ == nullptr) {
            throw std::bad_alloc();
        }

        // 取出一个空闲槽位
        Slot* slot = free_list_;
        free_list_ = free_list_->next;

        // placement new：在已有内存上构造对象
        T* obj = new (&slot->object) T(std::forward<Args>(args)...);

        return obj;
    }

    void destroy(T* obj) {
        if (obj == nullptr) {
            return;
        }

        // 手动调用析构函数
        obj->~T();

        // 把对象地址转换回 Slot 地址
        Slot* slot = reinterpret_cast<Slot*>(obj);

        // 放回空闲链表
        slot->next = free_list_;
        free_list_ = slot;
    }

    size_t capacity() const {
        return capacity_;
    }
};

int main() {
    ObjectPool<Order> pool(3);

    Order* o1 = pool.create(1, 10.5, 100);
    Order* o2 = pool.create(2, 20.5, 200);

    std::cout << o1->order_id << " " << o1->price << std::endl;
    std::cout << o2->order_id << " " << o2->price << std::endl;

    pool.destroy(o1);

    Order* o3 = pool.create(3, 30.5, 300);

    std::cout << o3->order_id << " " << o3->price << std::endl;

    pool.destroy(o2);
    pool.destroy(o3);

    return 0;
}