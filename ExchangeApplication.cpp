#include "ExchangeApplication.h"

std::string ExchangeApplication::outFilePath;
static std::ostringstream buffer;


void ExchangeApplication::writeExecutionReportsToFile(const ExecutionReport &executionReport) {
    std::ofstream file(outFilePath, std::ios_base::app);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + outFilePath);
    }

    std::string status = "New";
    if (executionReport.status == STATE_PFILLED) {
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

// To optimize the file I/O operations

//    buffer << executionReport.clientOrderId << " " << executionReport.orderId << " "
//           << executionReport.instrument << " " << executionReport.side << " "
//           << executionReport.price << " " << executionReport.quantity << " "
//           << status << " " << executionReport.reason << " " << executionReport.transactionTime << std::endl;
//
//
//    // Flush the buffer and write to the file if it exceeds a certain size or if you want to control when the write occurs
//    if (buffer.tellp() > 1024) { // Adjust the buffer size as needed
//        file.open(outFilePath, std::ios_base::app);
//        if (!file.is_open()) {
//            throw std::runtime_error("Error: Unable to open file " + outFilePath);
//        }
//
//        file << buffer.str();
//        file.close();
//        buffer.str("");
//    }
}

void ExchangeApplication::handleOrders(std::unordered_map<std::string, OrderBook *> &orderBooks,
                                       std::queue<Order> &ordersBuffer, sem_t &ordersSem, std::mutex &finishedMutex,
                                       bool &finished) {
    try {
        while (true) {
            finishedMutex.lock();
            if (finished && ordersBuffer.empty()) {
                finishedMutex.unlock();
                break;
            }
            finishedMutex.unlock();

            // Wait for a new order
            sem_wait(&ordersSem);

            // Get the order from the buffer
            Order order("", "", 0, 0.0, 0);

            if (!ordersBuffer.empty()) {
                order = ordersBuffer.front();
                ordersBuffer.pop();

                // Process the order in the OrderBook
                orderBooks[order.instrument]->processOrder(order);
            }
        }

    } catch (const std::exception &e) {
        std::cerr << "Error in consumer thread: " << e.what() << std::endl;
    }
}

void ExchangeApplication::setOutFilePath(std::string filePath) {
    ExchangeApplication::outFilePath = std::move(filePath);
}