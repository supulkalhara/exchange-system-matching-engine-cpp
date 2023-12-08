#include <thread>
#include <iostream>
#include <sstream>
#include <mutex>
#include <queue>
#include <semaphore.h>
#include "TraderApplication.h"
#include "ExchangeApplication.h"
#include "Order.h"
#include "OrderBook.h"
#include "ExecutionReport.h"

std::mutex bufferMutex;
std::queue<Order> ordersBuffer;
sem_t ordersSem;


int main() {

    // Initialize semaphore with 0 to start as locked
    sem_init(&ordersSem, 0, 0);
    std::string filePath = "D:\\lseg_project\\exchange-system-matching-engine-cpp\\inputs\\example5.csv";

    try {
        OrderBook orderBook;
        TraderApplication traderApp;
        ExchangeApplication exchangeApp;

        std::thread producer([&traderApp, filePath]() {
            traderApp.produceOrders(filePath, std::ref(ordersBuffer), std::ref(ordersSem), std::ref(bufferMutex));
        });

        std::thread consumer([&exchangeApp, &orderBook]() {
            exchangeApp.processOrders(orderBook, std::ref(ordersBuffer), std::ref(ordersSem), std::ref(bufferMutex));
        });

        producer.join();
        consumer.join();
    } catch (const std::exception& e) {
        std::cerr << "Error in main thread: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}