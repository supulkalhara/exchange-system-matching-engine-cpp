#include "ExchangeApplication.h"

int OrderBook::curId = 1;

OrderBook::OrderBook(const std::string instrument) : instrument(instrument) {}

void OrderBook::addOrderToBook(const Order &order, std::vector<Order> &orders, bool isBuyOrder) {
    orders.push_back(order);

    // Sort the order book based on the price
    std::sort(orders.begin(), orders.end(), [isBuyOrder](const Order &lhs, const Order &rhs) {
        return isBuyOrder ? (lhs.price < rhs.price) : (lhs.price > rhs.price);
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
        std::cerr << validationResult << " in the order: " << curOrderReport.clientOrderId  << std::endl;
        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
        return;
    }

    if (order.side == SIDE_SELL) {
        // Process sell orders
        processBuyOrders(order, curOrderReport);
    } else if (order.side == SIDE_BUY) {
        // Process buy orders
        processSellOrders(order, curOrderReport);
    }
    OrderBook::printOrderBook();
    std::cout << "************************************************************************************************" << std::endl;
}

void OrderBook::processSellOrders(Order &curOrder, ExecutionReport &curOrderReport) {
    matchingEngine(curOrder, curOrderReport, buyOrders, sellOrders);
}

void OrderBook::processBuyOrders(Order &curOrder, ExecutionReport &curOrderReport) {
    matchingEngine(curOrder, curOrderReport, sellOrders, buyOrders);
}

void OrderBook::matchingEngine(Order &curOrder, ExecutionReport &curOrderReport, std::vector<Order> &curSideOrders,
                              std::vector<Order> &oppositeSideOrders) {
    auto now = std::chrono::system_clock::now();
    std::string currentTime = getCurrentTime();
    curOrderReport.setTransactionTime(currentTime);

    int initialQuantity = curOrder.quantity;
    if (oppositeSideOrders.empty()) {
        // Not enough orders for matching, skip
        curOrderReport.price = curOrder.price;
        curOrderReport.quantity = curOrder.quantity;
        curOrderReport.status = STATE_NEW; // New

        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
        addOrderToBook(curOrder, curSideOrders, (curOrder.side == SIDE_BUY));
        return;
    }

    // Simplified matching logic (compare prices and execute if conditions met)
    for (size_t i = 0; i < oppositeSideOrders.size(); ++i) {
        Order &pendingOrder = oppositeSideOrders[i];

        if ((curOrder.side == SIDE_SELL && pendingOrder.price <= curOrder.price) ||
            (curOrder.side == SIDE_BUY && pendingOrder.price >= curOrder.price)) {

            executeOrder(curOrder, curOrderReport, pendingOrder, pendingOrder.side);
            updateOrderBook(oppositeSideOrders, i);

            if (curOrder.quantity == 0) {
                curOrderReport.status = STATE_FILLED; // Filled
                ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
                return;
            } else if (curOrder.quantity < initialQuantity) {
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

        ExchangeApplication::writeExecutionReportsToFile(curOrderReport);
        addOrderToBook(curOrder, curSideOrders, (curOrder.side == SIDE_BUY));
    } else {
        addOrderToBook(curOrder, curSideOrders, (curOrder.side == SIDE_BUY));
    }
}

void OrderBook::executeOrder(Order &curOrder, ExecutionReport &curOrderReport, Order &pendingOrder, int side) {
    curOrderReport.price = pendingOrder.price;
    curOrderReport.quantity = std::min(pendingOrder.quantity, curOrder.quantity);

    ExecutionReport matchedOrderReport(pendingOrder, 0, "");
    matchedOrderReport.setOrderId(pendingOrder.getOrderId());
    matchedOrderReport.setTransactionTime(getCurrentTime());
    matchedOrderReport.clientOrderId = pendingOrder.clientOrderId;
    matchedOrderReport.instrument = pendingOrder.instrument;
    matchedOrderReport.side = side;
    matchedOrderReport.price = pendingOrder.price;
    matchedOrderReport.quantity = curOrderReport.quantity;

    if (matchedOrderReport.quantity < pendingOrder.quantity) {
        matchedOrderReport.status = STATE_PFILLED; // PFilled
    } else {
        matchedOrderReport.status = STATE_FILLED; // Filled
    }

    ExchangeApplication::writeExecutionReportsToFile(matchedOrderReport);

    pendingOrder.quantity -= curOrderReport.quantity;
    curOrder.quantity -= curOrderReport.quantity;
}

void OrderBook::updateOrderBook(std::vector<Order> &orders, size_t &index) {
    if (orders[index].quantity == 0) {
        orders.erase(orders.begin() + index);
        --index;
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

