#include <iostream>
#include <queue>
#include <semaphore.h>
#include <functional>
#include "ExchangeApplication.h"
#include "OrderBook.h"

void ExchangeApplication::writeExecutionReportsToFile(const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + std::string(filename));
    }

    for (const auto& report : executionReports) {
        file << report.clientOrderId << " " << report.orderId << " " << report.instrument
             << " " << report.side << " " << report.price << " " << report.quantity
             << " " << report.status << " " << report.reason << " " << report.transactionTime << std::endl;
    }

    file.close();
}

void ExchangeApplication::processOrders(OrderBook orderBook, std::queue<Order>& ordersBuffer, sem_t& ordersSem, std::mutex& bufferMutex) {
    try {
        std::cout << "\nConsumer checking for orders..." << std::endl;

        while (true) {
            // Wait for a new order
            sem_wait(&ordersSem);
            std::cout << "\nConsumer is in the buffer..." << std::endl;

            // Get the order from the buffer
            Order order("", "", 0, 0.0, 0);
            {
                std::lock_guard<std::mutex> lock(bufferMutex);
                if (!ordersBuffer.empty()) {
                    order = ordersBuffer.front();
                    std::cout << "\n###  Order found: " << order.clientOrderId << "###" << std::endl;
                    ordersBuffer.pop();
                }
            }

            orderBook.addOrder(order);

            // Process the order in the OrderBook
            ExecutionReport executionReport(order, 0, "");
            orderBook.processOrders(executionReport, std::ref(executionReports));
        }

    } catch (const std::exception& e) {
        std::cerr << "Error in consumer thread: " << e.what() << std::endl;
    }

    std::cout << "\n########\nAll Orders have been processed...\n########\n" << std::endl;

    writeExecutionReportsToFile("execution_rep.csv");
}