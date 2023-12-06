#include "ExecutionReport.h"

ExecutionReport::ExecutionReport(const Order& order, int stat, const std::string& r)
        : clientOrderId(order.clientOrderId), instrument(order.instrument),
          side(order.side), price(order.price), quantity(order.quantity),
          status(stat), reason(r) {}

void ExecutionReport::setOrderId(const std::string& id) {
    orderId = id;
}

void ExecutionReport::setTransactionTime(const std::string& time) {
    transactionTime = time;
}
