// ExchangeApplication.h

#ifndef C___PROJECT_EXCHANGEAPPLICATION_H
#define C___PROJECT_EXCHANGEAPPLICATION_H

#include "Order.h"
#include <vector>
#include <mutex>
#include <fstream>
#include "ExecutionReport.h"
#include "OrderBook.h"

class ExchangeApplication {
private:
    std::vector<ExecutionReport> executionReports;
    std::mutex mtx;

public:
    void writeExecutionReportsToFile(const char *filename);

    void processOrders(OrderBook orderBook, std::queue<Order> &ordersBuffer, sem_t &ordersSem, std::mutex &bufferMutex);
};

#endif //C___PROJECT_EXCHANGEAPPLICATION_H
