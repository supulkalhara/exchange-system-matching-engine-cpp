#ifndef C___PROJECT_EXECUTIONREPORT_H
#define C___PROJECT_EXECUTIONREPORT_H

#include <string>
#include "Order.h"

class ExecutionReport {
public:
    std::string clientOrderId;
    std::string orderId;
    std::string instrument;
    int side;
    double price;
    int quantity;
    int status;
    std::string reason;
    std::string transactionTime;

    ExecutionReport(const Order& order, int stat, const std::string& r);

    void setOrderId(const std::string& id);

    void setTransactionTime(const std::string& time);
};

#endif //C___PROJECT_EXECUTIONREPORT_H