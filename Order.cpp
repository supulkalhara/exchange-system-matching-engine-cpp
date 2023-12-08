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
