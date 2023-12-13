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

std::mutex finishedMutex;
std::queue<Order> ordersBuffer;
sem_t ordersSem;
bool finished;

int main(int argc, char **argv) {
    if (argc != 3){
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }
    // Initialize semaphore with 0 to start as locked
    sem_init(&ordersSem, 0, 0);
    std::string inFilePath = argv[1];
    std::string outFilePath = argv[2];
//    "D:\\lseg_project\\exchange-system-matching-engine-cpp\\inputs\\example6.csv";
//   "D:\\lseg_project\\exchange-system-matching-engine-cpp\\outputs\\exec_report_example6.csv";
    finished = false;

    try {
        OrderBook orderBook;
        TraderApplication traderApp;
        ExchangeApplication exchangeApp;
        ExchangeApplication::setOutFilePath(outFilePath);

        std::thread producer([&traderApp, inFilePath]() {
            traderApp.produceOrders(inFilePath, std::ref(ordersBuffer), std::ref(ordersSem), std::ref(finishedMutex), finished);
        });

        std::thread consumer([&exchangeApp, &orderBook]() {
            exchangeApp.processOrders(orderBook, std::ref(ordersBuffer), std::ref(ordersSem), std::ref(finishedMutex), finished);
        });

        producer.join();
        consumer.join();
    } catch (const std::exception& e) {
        std::cerr << "Error in main thread: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}