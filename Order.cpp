#include "Order.h"

Order::Order(const std::string& id, const std::string& instr, int s, double p, int q)
        : clientOrderId(id), instrument(instr), side(s), price(p), quantity(q) {}

void Order::setOrderId(int id) {
    std::string finalId = "ord" + std::to_string(id);
    this->orderId = finalId;
}

std::string Order::getOrderId() {
    return this->orderId;
}

std::string Order::isValid() {

//    if (std::find(std::begin(val_instruments), std::end(val_instruments), this->instrument) == std::end(val_instruments)) {
//        return "Invalid instrument";
//    }
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
