// OrderBook.cpp

#include <iostream>
#include <algorithm>
#include "OrderBook.h"
#include "ExchangeApplication.h"

int OrderBook::curId = 1;

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
    order.setOrderId(OrderBook::curId);
    OrderBook::curId++;

    ExecutionReport curOrderReport(order, 0, "");
    curOrderReport.setOrderId(order.getOrderId());
    curOrderReport.clientOrderId = order.clientOrderId;
    curOrderReport.instrument = order.instrument;
    curOrderReport.side = order.side ; // Buy: 1, Sell: 2

    std::string validationResult = order.isValid();
    if (validationResult != "OK"){
        curOrderReport.price = order.price;
        curOrderReport.quantity = order.quantity;
        curOrderReport.status = STATE_REJECT; // Reject
        curOrderReport.reason = validationResult;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
        OrderBook::printOrderBook(order.instrument);
        return;
    }

    if (order.side == SIDE_SELL) {
        // Process sell orders
        processSellOrders(order, curOrderReport);
    } else if (order.side == SIDE_BUY) {
        // Process buy orders
        processBuyOrders(order, curOrderReport);
    }
    OrderBook::printOrderBook(order.instrument);
}

void OrderBook::processSellOrders(Order &curOrder, ExecutionReport &curOrderReport) {
    int initialQuantity = curOrder.quantity;
    std::cout << "\nMatching buy order..." << std::endl;
    std::vector<Order>&ordersForInstrument = sellOrders[curOrder.instrument];


    curOrderReport.setTransactionTime("20230101-120000.000");

    if (ordersForInstrument.empty()) {
        std::cout << "\nNot enough orders for matching, skip!" << std::endl;
        // Not enough orders for matching, skip
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = STATE_NEW; // New

        executionReports.push_back(curOrderReport);
        std::cout<<"Writing to the report"<<std::endl;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);

        addOrder(curOrder);
        return;
    }

    // Simplified matching logic (compare prices and execute if conditions met)
    for (size_t i = 0; i < ordersForInstrument.size(); ++i) {
        Order& sellOrder = ordersForInstrument[i];

        if (sellOrder.price <= curOrder.price) {
            std::cout << "\nsellOrder.price <= curOrder.price" << std::endl;

            // Execute the order (simplified execution)
            curOrderReport.price = sellOrder.price;
            curOrderReport.quantity = std::min(sellOrder.quantity, curOrder.quantity);

            ExecutionReport matchedOrderReport(sellOrder, 0, "");
            matchedOrderReport.setOrderId(sellOrder.getOrderId());
            matchedOrderReport.setTransactionTime("20230101-120000.000");
            matchedOrderReport.clientOrderId = sellOrder.clientOrderId;
            matchedOrderReport.instrument = sellOrder.instrument;
            matchedOrderReport.side = SIDE_SELL; // Buy: 1, Sell: 2
            matchedOrderReport.price = sellOrder.price;
            matchedOrderReport.quantity = std::min(sellOrder.quantity, curOrder.quantity);

            if (matchedOrderReport.quantity < sellOrder.quantity){
                matchedOrderReport.status = STATE_PFILLED; // PFilled
            } else {
                matchedOrderReport.status = STATE_FILLED; // Filled
            }

            ExchangeApplication::writeExecutionReportsToFile(matchedOrderReport);

            // Update order quantities (simplified logic)
            sellOrder.quantity -= curOrderReport.quantity;
            curOrder.quantity -= curOrderReport.quantity;

            // Remove filled orders
            if (sellOrder.quantity == 0) {
                ordersForInstrument.erase(ordersForInstrument.begin() + i);
                --i; // Adjust index after removal
            }

            if (curOrder.quantity == 0) {
                curOrderReport.status = STATE_FILLED; // Filled
                ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
                return;
            } else if (curOrder.quantity < initialQuantity){
                curOrderReport.status = STATE_PFILLED; // PFilled
                ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
            }
        } else {
            break;
        }
    }

    if (curOrder.quantity == initialQuantity) {
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = STATE_NEW; // New

        executionReports.push_back(curOrderReport);
        std::cout<<"Writing to the report"<<std::endl;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
        addOrder(curOrder);
    } else {
        addOrder(curOrder);
    }
}

void OrderBook::processBuyOrders(Order &curOrder, ExecutionReport &curOrderReport) {
    int initialQuantity = curOrder.quantity;
    std::cout << "\nMatching sell order..." << std::endl;
    std::vector<Order> &ordersForInstrument = buyOrders[curOrder.instrument];

    curOrderReport.setTransactionTime("20230101-120000.000");

    if (ordersForInstrument.empty()) {
        std::cout << "\nNot enough orders for matching, skip!" << std::endl;
        // Not enough orders for matching, skip
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = STATE_NEW; // New

        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);

        addOrder(curOrder);
        return;
    }

    // Simplified matching logic (compare prices and execute if conditions met)
    for (size_t i = 0; i < ordersForInstrument.size(); ++i) {
        Order &buyOrder = ordersForInstrument[i];

        if (buyOrder.price >= curOrder.price) {
            std::cout << "\nbuyOrder.price <= curOrder.price" << std::endl;

            // Execute the order (simplified execution)
            curOrderReport.price = buyOrder.price;
            curOrderReport.quantity = std::min(buyOrder.quantity, curOrder.quantity);

            ExecutionReport matchedOrderReport(buyOrder, 0, "");
            matchedOrderReport.setOrderId(buyOrder.getOrderId());
            matchedOrderReport.setTransactionTime("20230101-120000.000");
            matchedOrderReport.clientOrderId = buyOrder.clientOrderId;
            matchedOrderReport.instrument = buyOrder.instrument;
            matchedOrderReport.side = SIDE_BUY; // Buy: 1, Sell: 2
            matchedOrderReport.price = buyOrder.price;
            matchedOrderReport.quantity = std::min(buyOrder.quantity, curOrder.quantity);
            if (matchedOrderReport.quantity < buyOrder.quantity) {
                matchedOrderReport.status = STATE_PFILLED; // PFilled
            } else {
                matchedOrderReport.status = STATE_FILLED; // Filled
            }
            ExchangeApplication::writeExecutionReportsToFile(matchedOrderReport);

            // Update order quantities (simplified logic)
            buyOrder.quantity -= curOrderReport.quantity;
            curOrder.quantity -= curOrderReport.quantity;

            // Remove filled orders
            if (buyOrder.quantity == 0) {
                ordersForInstrument.erase(ordersForInstrument.begin() + i);
                --i; // Adjust index after removal
            }
            if (curOrder.quantity == 0) {
                curOrderReport.status = STATE_FILLED; // Filled
                ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
                return;
            } else if (curOrder.quantity < initialQuantity) {
                curOrderReport.status = STATE_PFILLED; // PFilled
                ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
            }
        }
    }

    if (curOrder.quantity == initialQuantity) {
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = STATE_NEW; // New

        executionReports.push_back(curOrderReport);
        std::cout << "Writing to the report" << std::endl;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
        addOrder(curOrder);
    } else {
        addOrder(curOrder);
    }
}

void OrderBook::printOrderBook(const std::string& instrument) {
    std::vector<Order> &buySide = buyOrders[instrument];
    std::vector<Order> &sellSide = sellOrders[instrument];

    std::cout << "*****************************************************************" << std::endl;

    std::cout << "Buy Side" << std::endl;
    std::cout << "OrderId\tQty\tPrice\t" << std::endl;
    for(auto &obj: buySide){
        std::cout << obj.getOrderId() << "\t" << obj.quantity << "\t" << obj.price << std::endl;
    }

    std::cout << "*****************************************************************" << std::endl;

    std::cout << "Sell Side" << std::endl;
    std::cout << "Price\tQty\tOrderId" << std::endl;
    for(auto &obj: sellSide){
        std::cout << obj.price << "\t" << obj.quantity << "\t" << obj.getOrderId() << std::endl;
    }

    std::cout << "*****************************************************************" << std::endl;

}
