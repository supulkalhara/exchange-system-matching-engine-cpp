#include <iostream>
#include <queue>
#include <semaphore.h>
#include <functional>
#include <string>
#include <utility>
#include "ExchangeApplication.h"
#include "OrderBook.h"

std::string ExchangeApplication::outFilePath;

void ExchangeApplication::writeExecutionReportsToFile(const ExecutionReport &executionReport) {
    std::ofstream file(outFilePath, std::ios_base::app);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + outFilePath);
    }

    std::string status = "New";
    if (executionReport.status == STATE_PFILLED){
        status = "PFill";
    } else if (executionReport.status == STATE_FILLED) {
        status = "Fill";
    } else if (executionReport.status == STATE_REJECT) {
        status = "Reject";
    }

    file << executionReport.clientOrderId << " " << executionReport.orderId << " " << executionReport.instrument
     << " " << executionReport.side << " " << executionReport.price << " " << executionReport.quantity
     << " " << status << " " << executionReport.reason << " " << executionReport.transactionTime << std::endl;
    file.close();
}

void ExchangeApplication::processOrders(OrderBook orderBook, std::queue<Order>& ordersBuffer, sem_t& ordersSem, std::mutex& finishedMutex, bool &finished) {
    try {
        std::cout << "\nConsumer checking for orders..." << std::endl;

        while (true) {
            finishedMutex.lock();
            if (finished && ordersBuffer.empty()) {
                finishedMutex.unlock();
                break;
            }
            finishedMutex.unlock();
            // Wait for a new order
            sem_wait(&ordersSem);
            std::cout << "\nConsumer is in the buffer..." << std::endl;

            // Get the order from the buffer
            Order order("", "", 0, 0.0, 0);
            if (!ordersBuffer.empty()) {
                order = ordersBuffer.front();
                std::cout << "\n###  Order found: " << order.clientOrderId << "###" << std::endl;
                ordersBuffer.pop();
            }

            // Process the order in the OrderBook
            orderBook.processOrders(order);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error in consumer thread: " << e.what() << std::endl;
    }

    std::cout << "\n########\nAll Orders have been processed...\n########\n" << std::endl;

}

 void ExchangeApplication::setOutFilePath(std::string filePath) {
    ExchangeApplication::outFilePath = filePath;
}