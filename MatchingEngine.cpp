#include <string>
#include <thread>
#include <mutex>
#include <iostream>
#include <deque>
#include <condition_variable>


class Order;
class Trade;

class OrderBook;

class MatchingEngine {
public:
    MatchingEngine(bool threaded = true) : threaded_(threaded) {
        if (threaded_) {
            // Start the processing thread
            std::thread processing_thread(&MatchingEngine::Run, this);
            processing_thread.detach();
        }
    }

    // Method to validate and process incoming orders
    void ProcessOrder(const Order& order) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (ValidateOrder(order)) {
            queue_.push_back(order);
            // Notify the processing thread that an order is available
            condition_.notify_one();
        } else {
            // Handle invalid order (e.g., log or notify)
            std::cerr << "Invalid order received: " << order.GetClientOrderId() << std::endl;
        }
        lock.unlock();
    }

    // Method to run in the processing thread
    void Run() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]{ return !queue_.empty(); });

            Order order = queue_.front();
            queue_.pop_front();
            lock.unlock();

            MatchOrder(order);
        }
    }

private:
    std::deque<Order> queue_;
    OrderBook orderbook_;
    std::deque<Trade> trades_;
    bool threaded_;
    std::mutex mutex_;
    std::condition_variable condition_;

    void Run() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]{ return !queue_.empty(); });

            // Process order
            // ... Implementation goes here

            lock.unlock();
        }
    }

    // Updated to use OrderBook's AddOrder
    void MatchOrder(const Order& order) {
        // Matching logic using the OrderBook
        // For example:
        if (order.GetSide() == "buy") {
            // Match with asks
            // ... implementation ...
        } else {
            // Match with bids
            // ... implementation ...
        }
    }

    // Helper method to validate an order
    bool ValidateOrder(const Order& order) {
        // Perform validation checks
        // Example: Check for valid instrument and quantity
        if (order.GetInstrument() != "Rose" && order.GetInstrument() != "Lavender") {
            return false;
        }
        if (order.GetQuantity() <= 0 || order.GetQuantity() % 10 != 0) {
            return false;
        }
        return true;
    }
};