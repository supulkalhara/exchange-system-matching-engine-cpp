#include "ExchangeApplication.h"

int OrderBook::curId = 1;

OrderBook::OrderBook(const std::string instrument) : instrument(instrument) {}

void OrderBook::addOrderToBook(const Order &order, std::vector<Order> &orders, bool isBuyOrder) {
    orders.push_back(order);

    // Sort the order book based on the price
    std::sort(orders.begin(), orders.end(), [isBuyOrder](const Order &lhs, const Order &rhs) {
        return isBuyOrder ? (lhs.price > rhs.price) : (lhs.price < rhs.price);
    });
}

void OrderBook::processOrder(Order &order) {
    order.setOrderId(OrderBook::curId);
    OrderBook::curId++;

    ExecutionReport curOrderReport(order, 0, "");
    curOrderReport.setOrderId(order.getOrderId());
    curOrderReport.clientOrderId = order.clientOrderId;
    curOrderReport.instrument = order.instrument;
    curOrderReport.side = order.side; // Buy: 1, Sell: 2

    std::string validationResult = order.isValid();
    if (validationResult != "OK") {
        curOrderReport.price = order.price;
        curOrderReport.quantity = order.quantity;
        curOrderReport.status = STATE_REJECT; // Reject
        curOrderReport.reason = validationResult;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
        OrderBook::printOrderBook();
        std::cerr << validationResult << " in the order: " << curOrderReport.clientOrderId  << std::endl;
        return;
    }

    if (order.side == SIDE_SELL) {
        // Process sell orders
        processSellOrders(order, curOrderReport);
    } else if (order.side == SIDE_BUY) {
        // Process buy orders
        processBuyOrders(order, curOrderReport);
    }
    OrderBook::printOrderBook();
}

void OrderBook::processSellOrders(Order &curOrder, ExecutionReport &curOrderReport) {
    std::vector<Order> &ordersForInstrument = sellOrders;
    matchingEngine(curOrder, curOrderReport, ordersForInstrument, SIDE_SELL);
}

void OrderBook::processBuyOrders(Order &curOrder, ExecutionReport &curOrderReport) {
    std::vector<Order> &ordersForInstrument = buyOrders;
    matchingEngine(curOrder, curOrderReport, ordersForInstrument, SIDE_BUY);
}

void OrderBook::matchingEngine(Order &curOrder, ExecutionReport &curOrderReport, std::vector<Order> &oppositeOrders,
                               int side) {
    auto now = std::chrono::system_clock::now();
    std::string currentTime = getCurrentTime();
    curOrderReport.setTransactionTime(currentTime);

    if (oppositeOrders.empty()) {
        // Not enough orders for matching, skip
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = STATE_NEW; // New

        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
        addOrderToBook(curOrder, oppositeOrders, (side == SIDE_BUY));
        return;
    }

    // Simplified matching logic (compare prices and execute if conditions met)
    for (size_t i = 0; i < oppositeOrders.size(); ++i) {
        Order &oppositeOrder = oppositeOrders[i];

        if ((side == SIDE_SELL && oppositeOrder.price <= curOrder.price) ||
            (side == SIDE_BUY && oppositeOrder.price >= curOrder.price)) {

            executeOrder(curOrder, curOrderReport, oppositeOrder);
            updateOrderBook(oppositeOrders, i);

            if (curOrder.quantity == 0) {
                curOrderReport.status = STATE_FILLED; // Filled
                ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
                return;
            } else if (curOrder.quantity < curOrderReport.quantity) {
                curOrderReport.status = STATE_PFILLED; // PFilled
                ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
            }
        } else {
            break;
        }
    }

    if (curOrder.quantity == curOrderReport.quantity) {
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = STATE_NEW; // New

        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
        addOrderToBook(curOrder, oppositeOrders, (side == SIDE_BUY));
    } else {
        addOrderToBook(curOrder, oppositeOrders, (side == SIDE_BUY));
    }
}

void OrderBook::executeOrder(Order &curOrder, ExecutionReport &curOrderReport, Order &oppositeOrder) {
    curOrderReport.price = oppositeOrder.price;
    curOrderReport.quantity = std::min(oppositeOrder.quantity, curOrder.quantity);

    ExecutionReport matchedOrderReport(oppositeOrder, 0, "");
    matchedOrderReport.setOrderId(oppositeOrder.getOrderId());
    matchedOrderReport.setTransactionTime(getCurrentTime());
    matchedOrderReport.clientOrderId = oppositeOrder.clientOrderId;
    matchedOrderReport.instrument = oppositeOrder.instrument;
    matchedOrderReport.side = curOrderReport.side;
    matchedOrderReport.price = oppositeOrder.price;
    matchedOrderReport.quantity = curOrderReport.quantity;

    if (matchedOrderReport.quantity < oppositeOrder.quantity) {
        matchedOrderReport.status = STATE_PFILLED; // PFilled
    } else {
        matchedOrderReport.status = STATE_FILLED; // Filled
    }

    ExchangeApplication::writeExecutionReportsToFile(matchedOrderReport);

    oppositeOrder.quantity -= curOrderReport.quantity;
    curOrder.quantity -= curOrderReport.quantity;
}

void OrderBook::updateOrderBook(std::vector<Order> &orders, size_t index) {
    if (orders[index].quantity == 0) {
        orders.erase(orders.begin() + index);
    }
}


void OrderBook::printOrderBook() {
    std::vector<Order> &buySide = buyOrders;
    std::vector<Order> &sellSide = sellOrders;

    std::cout << std::setw(78) << std::setfill('=') << "" << std::endl;

    // Buy Side Header
    std::cout << std::setfill(' ') << "| " << std::setw(18) << std::left << "Buy Side"
              << "| " << std::setw(20) << "OrderId"
              << "| " << std::setw(15) << "Qty"
              << "| " << std::setw(15) << "Price"
              << " |" << std::endl;

    std::cout << std::setw(78) << std::setfill('=') << "" << std::endl;

    // Buy Side Data
    for (const auto &obj: buySide) {
        std::cout << "| " << std::setw(18) << std::setfill(' ') << std::left << ""
                  << "| " << std::setw(20) << std::setfill(' ') << obj.getOrderId()
                  << "| " << std::setw(15) << std::setfill(' ') << obj.quantity
                  << "| " << std::setw(15) << std::setfill(' ') << obj.price
                  << " |" << std::endl;
    }

    std::cout << std::setw(78) << std::setfill('=') << "" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    // Sell Side Header
    std::cout << std::setw(78) << std::setfill('=') << "" << std::endl;

    std::cout << "| " << std::setw(18) << std::setfill(' ') << std::left << "Sell Side"
              << "| " << std::setw(15) << std::setfill(' ') << "Price"
              << "| " << std::setw(15) << std::setfill(' ') << "Qty"
              << "| " << std::setw(20) << std::setfill(' ') << "OrderId"
              << " |" << std::endl;

    std::cout << std::setw(78) << std::setfill('=') << "" << std::endl;

    // Sell Side Data
    for (const auto &obj: sellSide) {
        std::cout << "| " << std::setw(18) << std::setfill(' ') << std::left << ""
                  << "| " << std::setw(15) << std::setfill(' ') << obj.price
                  << "| " << std::setw(15) << std::setfill(' ') << obj.quantity
                  << "| " << std::setw(20) << std::setfill(' ') << obj.getOrderId()
                  << " |" << std::endl;
    }

    std::cout << std::setw(78) << std::setfill('=') << "" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
}

std::string OrderBook::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(now);
    struct std::tm *timeInfo = std::localtime(&currentTime);

    std::ostringstream oss;
    oss << std::put_time(timeInfo, "%Y%m%d-%H%M%S") << "." << std::setfill('0') << std::setw(3)
        << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    return oss.str();
}

