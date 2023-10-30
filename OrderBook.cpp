#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>
#include <utility>


class Side;
class Order;

class OrderBook {
public:
    OrderBook() {
        // Initialize books for different instruments
        books_["Rose"] = {std::vector<Order>(), std::vector<Order>()};
        books_["Lavender"] = {std::vector<Order>(), std::vector<Order>()};
        books_["Lotus"] = {std::vector<Order>(), std::vector<Order>()};
        books_["Tulip"] = {std::vector<Order>(), std::vector<Order>()};
        books_["Orchid"] = {std::vector<Order>(), std::vector<Order>()};
    }

    void AddOrder(const Order& order) {
        auto& book = books_[order.GetInstrument()];
        if (order.GetSide() == Side::Buy) {
            book.first.push_back(order);
            // Sort bids by price descending, then by client order ID ascending
            std::sort(book.first.begin(), book.first.end(), [](const Order& a, const Order& b) {
                return (a.GetPrice() > b.GetPrice()) ||
                       (a.GetPrice() == b.GetPrice() && a.GetClientOrderId() < b.GetClientOrderId());
            });
        } else {
            book.second.push_back(order);
            // Sort asks by price ascending, then by client order ID ascending
            std::sort(book.second.begin(), book.second.end(), [](const Order& a, const Order& b) {
                return (a.GetPrice() < b.GetPrice()) ||
                       (a.GetPrice() == b.GetPrice() && a.GetClientOrderId() < b.GetClientOrderId());
            });
        }
    }

    std::pair<std::vector<Order>, int> MatchOrder(const Order& incoming_order) {
        auto& book = books_[incoming_order.GetInstrument()];
        std::vector<Order> matched_orders;
        int remaining_quantity = incoming_order.GetQuantity();

        auto& opposite_side_orders = (incoming_order.GetSide() == Side::Buy) ? book.second : book.first;
        auto it = opposite_side_orders.begin();

        while (it != opposite_side_orders.end() && remaining_quantity > 0) {
            if ((incoming_order.GetSide() == Side::Buy && it->GetPrice() <= incoming_order.GetPrice()) ||
                (incoming_order.GetSide() == Side::Sell && it->GetPrice() >= incoming_order.GetPrice())) {
                int traded_quantity = std::min(it->GetQuantity(), remaining_quantity);
                remaining_quantity -= traded_quantity;
                it->SetQuantity(it->GetQuantity() - traded_quantity);
                matched_orders.push_back(*it);

                if (it->GetQuantity() == 0) {
                    it = opposite_side_orders.erase(it);
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
        return {matched_orders, remaining_quantity};
    }

    void RemoveOrder(const std::string& client_order_id, const std::string& instrument) {
        auto& book = books_[instrument];
        for (auto& side : {&book.first, &book.second}) {
            side->erase(std::remove_if(side->begin(), side->end(),
                                       [&client_order_id](const Order& order) {
                                           return order.GetClientOrderId() == client_order_id;
                                       }), side->end());
        }
    }

private:
    std::map<std::string, std::pair<std::vector<Order>, std::vector<Order>>> books_;
};
