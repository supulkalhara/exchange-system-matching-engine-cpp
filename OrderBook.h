//
// Created by agskp on 12/6/2023.
//

#ifndef C___PROJECT_ORDERBOOK_H
#define C___PROJECT_ORDERBOOK_H

#include <unordered_map>
#include <vector>
#include "Order.h"
#include "ExecutionReport.h"

class OrderBook {
private:
    std::unordered_map<std::string, std::vector<Order>> buyOrders; // Map of instrument to buy orders
    std::unordered_map<std::string, std::vector<Order>> sellOrders; // Map of instrument to sell orders

public:
    void addBuyOrder(const Order& order);
    void addSellOrder(const Order& order);
    void processOrders(ExecutionReport& executionReport);
};

#endif //C___PROJECT_ORDERBOOK_H
