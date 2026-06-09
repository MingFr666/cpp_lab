#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <thread>

struct OrderRequest
{
    std::string order_id;
    std::string symbol;
    int qty;
    double price;
};

class OrderSender {
private:
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stopped_;
    std::queue<OrderRequest> order_queue;
    std::thread worker_thread_;

public:
    OrderSender() : stopped_(false) {
        worker_thread_ = std::thread(&OrderSender::run, this);
    }

    ~OrderSender() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stopped_ = true;
        }
        cv_.notify_all();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
    }

    void insert_order(const OrderRequest& order) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            order_queue.push(order);
        }
        cv_.notify_one();
    }

private:
    void run() {
        while (true)
        {
            OrderRequest order;
            
            {
                std::unique_lock<std::mutex> lock(mtx_);
                cv_.wait(lock, [this]{
                    return stopped_ || order_queue.empty();
                });

                if (stopped_ && order_queue.empty())
                {
                    break;
                }

                order = std::move(order_queue.front());
                order_queue.pop();
            }

            bool ok = send_to_trade_node(order);

            if (!ok)
            {
                std::cerr << "Failed to send order " << order.order_id << '\n';
            }
        }       
    }

    bool send_to_trade_node(const OrderRequest& order) {
        // 真实项目里这里可能是 TCP / ZMQ / HTTP / 券商 API
        std::cout << "send order " << order.order_id << '\n';
        return true;
    }
};
