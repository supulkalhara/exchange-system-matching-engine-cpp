#include "TraderApplication.h"


void TraderApplication::produceOrders(const std::string &filePath, std::queue<Order> &ordersBuffer, sem_t &ordersSem,
                                      std::mutex &finishedMutex, bool &finished) {
    try {
        std::ifstream file(filePath);

        if (!file.is_open()) {
            throw std::runtime_error("Error: Unable to open file 'example6.csv'");
        }

        // Skip the header line
        std::string header;
        std::getline(file, header);

        std::string line;
        while (std::getline(file, line)) {
            // Parse CSV line and create Order
            std::stringstream lineStream(line);
            std::string clientOrderId, instrument, sideStr, priceStr, quantityStr;

            int side, quantity;
            double price;

            std::getline(lineStream, clientOrderId, ',');
            std::getline(lineStream, instrument, ',');
            std::getline(lineStream, sideStr, ',');
            std::getline(lineStream, priceStr, ',');
            std::getline(lineStream, quantityStr, ',');

            // Convert side, quantity, and price to appropriate types
            side = std::stoi(sideStr);
            price = std::stod(priceStr);
            quantity = std::stoi(quantityStr);

            Order order(clientOrderId, instrument, side, price, quantity);

            std::cout << "Producing Order: "
                      << "Client Order ID: " << order.clientOrderId
                      << ", Instrument: " << order.instrument
                      << ", Side: " << order.side
                      << ", Price: " << order.price
                      << ", Quantity: " << order.quantity
                      << std::endl;

            ordersBuffer.push(order);

            // Notify the consumer about the new order
            sem_post(&ordersSem);
        }
    } catch (const std::exception &e) {
        std::cerr << "Error in producer thread: " << e.what() << std::endl;
    }
    finishedMutex.lock();
    finished = true;
    std::cout << "\nProducer completed reading orders\n" << std::endl;
    finishedMutex.unlock();
}
