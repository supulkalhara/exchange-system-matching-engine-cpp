// OrderBook.cpp

#include <iostream>
#include "OrderBook.h"

void OrderBook::addBuyOrder(const Order& order) {
    buyOrders[order.instrument].push_back(order);
}

void OrderBook::addSellOrder(const Order& order) {
    sellOrders[order.instrument].push_back(order);
}

void OrderBook::processOrders(ExecutionReport& executionReport) {
    // Simplified order processing logic (matching buy and sell orders)
    for (const auto& instrumentEntry : buyOrders) {
        const std::string& instrument = instrumentEntry.first;
        std::vector<Order> buyOrdersForInstrument = instrumentEntry.second;

        if (sellOrders.find(instrument) != sellOrders.end()) {
            std::vector<Order>& sellOrdersForInstrument = sellOrders[instrument];

            for (Order &buyOrder : buyOrdersForInstrument) {
                for (Order &sellOrder : sellOrdersForInstrument) {
                    // Simplified matching logic, compare prices and execute if conditions met
                    if (buyOrder.price >= sellOrder.price) {
                        // Execute the order (simplified execution)
                        executionReport.setOrderId("SystemGeneratedID");
                        executionReport.setTransactionTime("20230101-120000.000");
                        executionReport.clientOrderId = buyOrder.clientOrderId;
                        executionReport.orderId = sellOrder.clientOrderId;
                        executionReport.instrument = instrument;
                        executionReport.side = 1; // Buy
                        executionReport.price = buyOrder.price;
                        executionReport.quantity = std::min(buyOrder.quantity, sellOrder.quantity);
                        executionReport.status = 2; // Filled

                        // Update order quantities (simplified logic)
                        buyOrder.quantity -= executionReport.quantity;
                        sellOrder.quantity -= executionReport.quantity;

                        // Remove filled orders
                        if (buyOrder.quantity == 0) {
                            // Remove buy order if fully filled
                            buyOrdersForInstrument.pop_back();
                        }
                        if (sellOrder.quantity == 0) {
                            // Remove sell order if fully filled
                            sellOrdersForInstrument.pop_back();
                        }

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
}
