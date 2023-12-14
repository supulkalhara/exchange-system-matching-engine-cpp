#ifndef C___PROJECT_EXCHANGEAPPLICATION_H
#define C___PROJECT_EXCHANGEAPPLICATION_H

#include <vector>
#include <mutex>
#include <fstream>
#include <queue>
#include <iostream>
#include <iostream>
#include <semaphore.h>
#include <functional>
#include "OrderBook.h"
#include "Order.h"
#include "ExecutionReport.h"

class ExchangeApplication {
private:
    std::vector<ExecutionReport> executionReports;
    static std::string outFilePath;
public:
    static void writeExecutionReportsToFile(const ExecutionReport &executionReport);
    static void setOutFilePath(std::string filePath);

    static void handleOrders(std::unordered_map<std::string, OrderBook *> &orderBooks, std::queue<Order> &ordersBuffer,
                      sem_t &ordersSem, std::mutex &finishedMutex, bool &finished);
};

#endif //C___PROJECT_EXCHANGEAPPLICATION_H
