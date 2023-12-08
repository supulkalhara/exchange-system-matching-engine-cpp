// OrderBook.cpp

#include <iostream>
#include <mutex>
#include <algorithm>
#include "OrderBook.h"
#include "ExchangeApplication.h"

std::unordered_map<std::string, int> OrderBook::curIds = { { "Rose", 1 }, { "Lavender", 1}, { "Lotus", 1 }, { "Tulip", 1}, { "Orchid", 1}};

void OrderBook::addOrder(const Order& order) {
    if (order.side == 1) {
        // Add buy order
        addBuyOrder(order);
    } else if (order.side == 2) {
        // Add sell order
        addSellOrder(order);
    }
    std::cout << "\nEntered to OrderBook..." << std::endl;
}

void OrderBook::addBuyOrder(const Order& order) {
    // Add buy order to the corresponding instrument's buy order book
    buyOrders[order.instrument].push_back(order);
    std::sort(buyOrders[order.instrument].begin(), buyOrders[order.instrument].end(),[ ]( const Order& lhs, const Order& rhs )
    {
        return lhs.price > rhs.price;
    });
    // Log the addition of a buy order
    std::cout << "Buy Order Added: "
              << "Client Order ID: " << order.clientOrderId
              << ", Instrument: " << order.instrument
              << ", Price: " << order.price
              << ", Quantity: " << order.quantity
              << std::endl;
}

void OrderBook::addSellOrder(const Order& order) {
    // Add sell order to the corresponding instrument's sell order book
    sellOrders[order.instrument].push_back(order);
    std::sort(sellOrders[order.instrument].begin(), sellOrders[order.instrument].end(),[ ]( const Order& lhs, const Order& rhs )
    {
        return lhs.price < rhs.price;
    });
    // Log the addition of a sell order
    std::cout << "Sell Order Added: "
              << "Client Order ID: " << order.clientOrderId
              << ", Instrument: " << order.instrument
              << ", Price: " << order.price
              << ", Quantity: " << order.quantity
              << std::endl;
}

void OrderBook::processOrders(Order& order) {
    std::cout<<"Order side: "<<order.side<<std::endl;
    order.setOrderId(OrderBook::curIds[order.instrument]);
    OrderBook::curIds[order.instrument]++;
    if (order.side == 1) {
        // Process sell orders
        processSellOrders(order);
    } else if (order.side == 2) {
        // Process buy orders
        processBuyOrders(order);

    }
}

void OrderBook::processSellOrders(Order &curOrder) {
    ExecutionReport curOrderReport(curOrder, 0, "");
    std::cout << "\nMatching buy order..." << std::endl;
    std::vector<Order>&ordersForInstrument = sellOrders[curOrder.instrument];

    curOrderReport.setOrderId(curOrder.getOrderId());
    curOrderReport.setTransactionTime("20230101-120000.000");
    curOrderReport.clientOrderId = curOrder.clientOrderId;
    curOrderReport.instrument = curOrder.instrument;
    curOrderReport.side = (&sellOrders == &buyOrders) ? 1 : 2; // Buy: 1, Sell: 2

    if (ordersForInstrument.empty()) {
        std::cout << "\nNot enough orders for matching, skip!" << std::endl;
        // Not enough orders for matching, skip
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = 0; // New

        executionReports.push_back(curOrderReport);
        std::cout<<"Writing to the report"<<std::endl;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport, ExchangeApplication::outFilePath);

        addOrder(curOrder);
        return;
    }

    // Simplified matching logic (compare prices and execute if conditions met)
    for (size_t i = 0; i < ordersForInstrument.size() - 1; ++i) {
        Order& sellOrder = ordersForInstrument[i];

        if (sellOrder.price <= curOrder.price) {
            std::cout << "\nsellOrder.price <= curOrder.price" << std::endl;

            // Execute the order (simplified execution)
            curOrderReport.price = sellOrder.price;
            curOrderReport.quantity = std::min(sellOrder.quantity, curOrder.quantity);
            curOrderReport.status = 2; // Filled
            ExchangeApplication::writeExecutionReportsToFile(curOrderReport, ExchangeApplication::outFilePath);

            ExecutionReport matchedOrderReport(sellOrder, 0, "");
            matchedOrderReport.setOrderId(sellOrder.getOrderId());
            matchedOrderReport.setTransactionTime("20230101-120000.000");
            matchedOrderReport.clientOrderId = curOrder.clientOrderId;
            matchedOrderReport.instrument = curOrder.instrument;
            matchedOrderReport.side = 2; // Buy: 1, Sell: 2
            matchedOrderReport.price = sellOrder.price;
            matchedOrderReport.quantity = std::min(sellOrder.quantity, curOrder.quantity);
            matchedOrderReport.status = 2; // Filled
            ExchangeApplication::writeExecutionReportsToFile(matchedOrderReport, ExchangeApplication::outFilePath);

            // Update order quantities (simplified logic)
            sellOrder.quantity -= curOrderReport.quantity;
            curOrder.quantity -= curOrderReport.quantity;

            // Remove filled orders
            if (sellOrder.quantity == 0) {
                ordersForInstrument.erase(ordersForInstrument.begin() + i);
                --i; // Adjust index after removal
            }

            // Print execution report (you can modify this part based on your needs)
            std::cout << "Execution Report: "
                      << "Client Order ID: " << curOrderReport.clientOrderId
                      << ", Order ID: " << curOrderReport.orderId
                      << ", Instrument: " << curOrderReport.instrument
                      << ", Side: " << curOrderReport.side
                      << ", Price: " << curOrderReport.price
                      << ", Quantity: " << curOrderReport.quantity
                      << ", Status: " << curOrderReport.status
                      << ", Transaction Time: " << curOrderReport.transactionTime
                      << std::endl;
            if (curOrder.quantity == 0) {
                break;
            }
        } else {
            break;
        }
    }

    if (curOrder.quantity != 0) {
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = 0; // New

        executionReports.push_back(curOrderReport);
        std::cout<<"Writing to the report"<<std::endl;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport, ExchangeApplication::outFilePath);
        addOrder(curOrder);
    }
}

void OrderBook::processBuyOrders(Order &curOrder) {
    ExecutionReport curOrderReport(curOrder, 0, "");
    std::cout << "\nMatching sell order..." << std::endl;
    std::vector<Order>&ordersForInstrument = buyOrders[curOrder.instrument];

    curOrderReport.setOrderId(curOrder.getOrderId());
    curOrderReport.setTransactionTime("20230101-120000.000");
    curOrderReport.clientOrderId = curOrder.clientOrderId;
    curOrderReport.instrument = curOrder.instrument;
    curOrderReport.side = (&sellOrders == &buyOrders) ? 1 : 2; // Buy: 1, Sell: 2

    if (ordersForInstrument.empty()) {
        std::cout << "\nNot enough orders for matching, skip!" << std::endl;
        // Not enough orders for matching, skip
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = 0; // New

        executionReports.push_back(curOrderReport);
        std::cout<<"Writing to the report"<<std::endl;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport, ExchangeApplication::outFilePath);

        addOrder(curOrder);
        return;
    }

    // Simplified matching logic (compare prices and execute if conditions met)
    for (size_t i = 0; i < ordersForInstrument.size() - 1; ++i) {
        Order& buyOrder = ordersForInstrument[i];

        if (buyOrder.price >= curOrder.price) {
            std::cout << "\nbuyOrder.price <= curOrder.price" << std::endl;

            // Execute the order (simplified execution)
            curOrderReport.price = buyOrder.price;
            curOrderReport.quantity = std::min(buyOrder.quantity, curOrder.quantity);
            curOrderReport.status = 2; // Filled
            ExchangeApplication::writeExecutionReportsToFile(curOrderReport, ExchangeApplication::outFilePath);

            ExecutionReport matchedOrderReport(buyOrder, 0, "");
            matchedOrderReport.setOrderId(buyOrder.getOrderId());
            matchedOrderReport.setTransactionTime("20230101-120000.000");
            matchedOrderReport.clientOrderId = curOrder.clientOrderId;
            matchedOrderReport.instrument = curOrder.instrument;
            matchedOrderReport.side = 2; // Buy: 1, Sell: 2
            matchedOrderReport.price = buyOrder.price;
            matchedOrderReport.quantity = std::min(buyOrder.quantity, curOrder.quantity);
            matchedOrderReport.status = 2; // Filled
            ExchangeApplication::writeExecutionReportsToFile(matchedOrderReport, ExchangeApplication::outFilePath);

            // Update order quantities (simplified logic)
            buyOrder.quantity -= curOrderReport.quantity;
            curOrder.quantity -= curOrderReport.quantity;

            // Remove filled orders
            if (buyOrder.quantity == 0) {
                ordersForInstrument.erase(ordersForInstrument.begin() + i);
                --i; // Adjust index after removal
            }
            if (curOrder.quantity == 0) {
                break;
            }

            // Print execution report (you can modify this part based on your needs)
            std::cout << "Execution Report: "
                      << "Client Order ID: " << curOrderReport.clientOrderId
                      << ", Order ID: " << curOrderReport.orderId
                      << ", Instrument: " << curOrderReport.instrument
                      << ", Side: " << curOrderReport.side
                      << ", Price: " << curOrderReport.price
                      << ", Quantity: " << curOrderReport.quantity
                      << ", Status: " << curOrderReport.status
                      << ", Transaction Time: " << curOrderReport.transactionTime
                      << std::endl;
        }
    }

    if (curOrder.quantity != 0) {
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = 0; // New

        executionReports.push_back(curOrderReport);
        std::cout<<"Writing to the report"<<std::endl;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport, ExchangeApplication::outFilePath);
        addOrder(curOrder);
    }
}