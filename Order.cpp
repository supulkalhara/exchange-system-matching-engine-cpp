#include "Order.h"

Order::Order(const std::string& id, const std::string& instr, int s, double p, int q)
        : clientOrderId(id), instrument(instr), side(s), price(p), quantity(q) {}
