#include <thread>
#include <iostream>
#include <mutex>
#include <queue>
#include <semaphore.h>
#include "TraderApplication.h"
#include "ExchangeApplication.h"
#include "Order.h"
#include "OrderBook.h"

std::mutex finishedMutex;
std::queue<Order> ordersBuffer;
sem_t ordersSem;
bool finished;


int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }

    // Initialize semaphore with 0 to start as locked
    sem_init(&ordersSem, 0, 0);

    std::string inFilePath = argv[1]; //    "{path to the src file}\\inputs\\orders.csv";
    std::string outFilePath = argv[2]; //    "{path to the src file}\\outputs\\execution_rep.csv";

    finished = false;

    try {
        std::unordered_map<std::string, OrderBook *> orderBooks;
        TraderApplication traderApp;
        ExchangeApplication exchangeApp;
        ExchangeApplication::setOutFilePath(outFilePath);

        for (const std::string &instrument: instruments) {
            auto *ob = new OrderBook(instrument);
            orderBooks.emplace(instrument, ob);
        }

        std::thread producer([&traderApp, inFilePath]() {
            traderApp.produceOrders(inFilePath, std::ref(ordersBuffer), std::ref(ordersSem), std::ref(finishedMutex),
                                    finished);
        });

        std::thread consumer([&exchangeApp, &orderBooks]() {
            exchangeApp.handleOrders(orderBooks, std::ref(ordersBuffer), std::ref(ordersSem), std::ref(finishedMutex),
                                     finished);
        });

        producer.join();
        consumer.join();
        std::cout << "\n########\nExecution Report Generated!\n########\n" << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error in main thread: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}