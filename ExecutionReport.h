#ifndef C___PROJECT_EXECUTIONREPORT_H
#define C___PROJECT_EXECUTIONREPORT_H

#include <string>
#include "Order.h"

#define STATE_NEW 0
#define STATE_PFILLED 1
#define STATE_FILLED 2
#define STATE_REJECT 3

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
