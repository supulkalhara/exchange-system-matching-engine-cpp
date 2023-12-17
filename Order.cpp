#include "Order.h"

Order::Order(std::string id, std::string instr, int s, double p, int q)
        : clientOrderId(std::move(id)), instrument(std::move(instr)), side(s), price(p), quantity(q) {}

void Order::setOrderId(int id) {
    std::string finalId = "ord" + std::to_string(id);
    this->orderId = finalId;
}

std::string Order::getOrderId() const {
    return this->orderId;
}

std::string Order::isValid() const {

    if (std::find(std::begin(instruments), std::end(instruments), this->instrument) == std::end(instruments)) {
        return "Invalid instrument";
    }

    if (this->side != SIDE_SELL && this->side != SIDE_BUY) {
        return "Invalid side";
    }

    if (this->price <= 0) {
        return "Invalid price";
    }

    if (this->quantity % 10 != 0 || this->quantity < 10 || this->quantity > 1000) {
        return "Invalid size";
    }

    return "OK";
}
