// OrderBook.cpp

#include <iostream>
#include <mutex>
#include "OrderBook.h"


void OrderBook::addOrder(const Order& order) {
    if (order.side == 0) {
        // Add buy order
        addBuyOrder(order);
    } else if (order.side == 1) {
        // Add sell order
        addSellOrder(order);
    }
}

void OrderBook::addBuyOrder(const Order& order) {
    // Add buy order to the corresponding instrument's buy order book
    buyOrders[order.instrument].push_back(order);

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

    // Log the addition of a sell order
    std::cout << "Sell Order Added: "
              << "Client Order ID: " << order.clientOrderId
              << ", Instrument: " << order.instrument
              << ", Price: " << order.price
              << ", Quantity: " << order.quantity
              << std::endl;
}

void OrderBook::processOrders(ExecutionReport& executionReport, std::vector<ExecutionReport> executionReports) {
    std::cout << "\nEntered to OrderBook..." << std::endl;
    std::cout << "\nMatching buy and sell orders..." << std::endl;

    // Process both buy and sell orders
    processSideOrders(buyOrders, executionReport);
    processSideOrders(sellOrders, executionReport);

}

void OrderBook::processSideOrders(std::unordered_map<std::string, std::vector<Order>>& sideOrders, ExecutionReport& executionReport) {
    for (auto& instrumentEntry : sideOrders) {
        const std::string& instrument = instrumentEntry.first;
        std::vector<Order>& ordersForInstrument = sideOrders[instrument];

        if (ordersForInstrument.size() < 2) {
            std::cout << "\nNot enough orders for matching, skip!" << std::endl;
            // Not enough orders for matching, skip
            continue;
        }

        // Simplified matching logic (compare prices and execute if conditions met)
        for (size_t i = 0; i < ordersForInstrument.size() - 1; ++i) {
            for (size_t j = i + 1; j < ordersForInstrument.size(); ++j) {
                Order& order1 = ordersForInstrument[i];
                Order& order2 = ordersForInstrument[j];

                if (order1.price >= order2.price) {
                    std::cout << "\norder1.price >= order2.price" << std::endl;

                    // Execute the order (simplified execution)
                    executionReport.setOrderId("SystemGeneratedID");
                    executionReport.setTransactionTime("20230101-120000.000");
                    executionReport.clientOrderId = order1.clientOrderId;
                    executionReport.orderId = order2.clientOrderId;
                    executionReport.instrument = instrument;
                    executionReport.side = (&sideOrders == &buyOrders) ? 1 : 2; // Buy: 1, Sell: 2
                    executionReport.price = order1.price;
                    executionReport.quantity = std::min(order1.quantity, order2.quantity);
                    executionReport.status = 2; // Filled

                    // Update order quantities (simplified logic)
                    order1.quantity -= executionReport.quantity;
                    order2.quantity -= executionReport.quantity;

                    // Remove filled orders
                    if (order1.quantity == 0) {
                        ordersForInstrument.erase(ordersForInstrument.begin() + i);
                        --i; // Adjust index after removal
                    }
                    if (order2.quantity == 0) {
                        ordersForInstrument.erase(ordersForInstrument.begin() + j);
                        --j; // Adjust index after removal
                    }

                    std::cout << "\nAdded one to the reports list" << std::endl;
                    executionReports.push_back(executionReport);

                    // Print execution report (you can modify this part based on your needs)
                    std::cout << "Execution Report: "
                              << "Client Order ID: " << executionReport.clientOrderId
                              << ", Order ID: " << executionReport.orderId
                              << ", Instrument: " << executionReport.instrument
                              << ", Side: " << executionReport.side
                              << ", Price: " << executionReport.price
                              << ", Quantity: " << executionReport.quantity
                              << ", Status: " << executionReport.status
                              << ", Transaction Time: " << executionReport.transactionTime
                              << std::endl;
                }
            }
        }
    }
}
