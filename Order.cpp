#include <algorithm>
#include "Order.h"

Order::Order(const std::string& id, const std::string& instr, int s, double p, int q)
        : clientOrderId(id), instrument(instr), side(s), price(p), quantity(q), instrumentList({"Rose", "Lavender", "Lotus", "Tulip", "Orchid"}) {}

void Order::setOrderId(int id) {
    std::string finalId = "ord" + std::to_string(id);
    this->orderId = finalId;
}

std::string Order::getOrderId() {
    return this->orderId;
}

std::string Order::isValid() {

    auto it = std::find(instrumentList.begin(), instrumentList.end(), this->instrument);
    if (it == instrumentList.end()){
        return "Invalid instrument";
    }
    if ((this->side != 1) && (this->side != 2)){
        return "Invalid side";
    }
    if (this->quantity%10 != 0){
        return "Invalid size";
    }
    if ((this->side != 1) && (this->side != 2)){
        return "Invalid size";
    }
    if (this->price <= 0){
        return "Invalid price";
    }
    if ((this->quantity <= 10) || (this->quantity >= 1000)){
        return "Invalid size";
    }

    return "OK";
}