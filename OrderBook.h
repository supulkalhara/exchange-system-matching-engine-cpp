#ifndef C___PROJECT_ORDERBOOK_H
#define C___PROJECT_ORDERBOOK_H

#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include "Order.h"
#include "ExecutionReport.h"

class OrderBook {
private:
    std::vector<Order> buyOrders; // Map of instrument to buy orders
    std::vector<Order> sellOrders; // Map of instrument to sell orders
    std::vector<ExecutionReport> executionReports;  // Vector to store execution reports
    static int curId;
    std::string instrument;

public:
    explicit OrderBook(const std::string instrument);

    void processBuyOrders(Order &curOrder, ExecutionReport &curOrderReport);

    void processSellOrders(Order &curOrder, ExecutionReport &curOrderReport);

    void processOrder(Order &order);

    void printOrderBook();

    void updateOrderBook(std::vector<Order> &orders, size_t index);

    void executeOrder(Order &curOrder, ExecutionReport &curOrderReport, Order &oppositeOrder);

    void matchingEngine(Order &curOrder, ExecutionReport &curOrderReport, std::vector<Order> &oppositeOrders, int side);

    std::string getCurrentTime();

    void addOrderToBook(const Order &order, std::vector<Order> &orders, bool isBuyOrder);
};

#endif //C___PROJECT_ORDERBOOK_H
