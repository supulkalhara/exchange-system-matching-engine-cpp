#ifndef C___PROJECT_ORDER_H
#define C___PROJECT_ORDER_H

#include <string>
#include <vector>
#include <algorithm>

#include "utils.h"

#define SIDE_SELL 1
#define  SIDE_BUY 2

class Order {
private:
    std::string orderId;
public:
    std::string clientOrderId;
    std::string instrument;
    int side;
    double price;
    int quantity;
    void setOrderId(int id);
    std::string getOrderId();

    Order(const std::string& id, const std::string& instr, int s, double p, int q);

    std::string isValid();
};

#endif //C___PROJECT_ORDER_H
