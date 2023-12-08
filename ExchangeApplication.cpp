#include <iostream>
#include <queue>
#include <semaphore.h>
#include <functional>
#include <string>
#include "ExchangeApplication.h"
#include "OrderBook.h"

std::string ExchangeApplication::outFilePath =  "D:\\lseg_project\\exchange-system-matching-engine-cpp\\outputs\\exec_report_example6.csv";

void ExchangeApplication::writeExecutionReportsToFile(const ExecutionReport &executionReport, const std::string &filename) {
    std::ofstream file(filename, std::ios_base::app);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }

    std::string status = "New";
    if (executionReport.status == 1){
        status = "PFill";
    } else if (executionReport.status == 2) {
        status = "Fill";
    } else if (executionReport.status == 3) {
        status = "Reject";
    }

    file << executionReport.clientOrderId << " " << executionReport.orderId << " " << executionReport.instrument
     << " " << executionReport.side << " " << executionReport.price << " " << executionReport.quantity
     << " " << status << " " << executionReport.reason << " " << executionReport.transactionTime << std::endl;
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

//            orderBook.addOrder(order);

            // Process the order in the OrderBook
            orderBook.processOrders(order);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error in consumer thread: " << e.what() << std::endl;
    }

    std::cout << "\n########\nAll Orders have been processed...\n########\n" << std::endl;

//    writeExecutionReportsToFile("execution_rep.csv");
}