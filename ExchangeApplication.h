// ExchangeApplication.h

#ifndef C___PROJECT_EXCHANGEAPPLICATION_H
#define C___PROJECT_EXCHANGEAPPLICATION_H

#include "Order.h"
#include <vector>
#include <mutex>
#include <fstream>
#include <queue>
#include <semaphore.h>
#include "ExecutionReport.h"
#include "OrderBook.h"

class ExchangeApplication {
private:
    std::vector<ExecutionReport> executionReports;
    std::mutex mtx;
    static std::string outFilePath;
public:
    static void writeExecutionReportsToFile(const ExecutionReport &executionReport);
    static void setOutFilePath(std::string filePath);
    void processOrders(OrderBook orderBook, std::queue<Order> &ordersBuffer, sem_t &ordersSem, std::mutex &bufferMutex, bool &finished);
};

#endif //C___PROJECT_EXCHANGEAPPLICATION_H
