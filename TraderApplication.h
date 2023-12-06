#ifndef C___PROJECT_TRADERAPPLICATION_H
#define C___PROJECT_TRADERAPPLICATION_H

#include "Order.h"
#include <vector>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <queue>

class TraderApplication {
private:
    std::queue<Order> orderBuffer;
    std::condition_variable cv;

public:

    void
    produceOrders(const std::string &filePath, std::queue<Order> &ordersBuffer, sem_t &ordersSem,
                  std::mutex &bufferMutex);
};

#endif //C___PROJECT_TRADERAPPLICATION_H
