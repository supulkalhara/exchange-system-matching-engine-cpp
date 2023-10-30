#include <string>

class Order {
public:
    // Constructor
    Order(const std::string& client_order_id, const std::string& instrument, int side, double price, int quantity)
            : client_order_id_(client_order_id), instrument_(instrument), side_(side), price_(price), quantity_(quantity) {}

    // Accessors
    const std::string& GetClientOrderId() const { return client_order_id_; }
    const std::string& GetInstrument() const { return instrument_; }
    int GetSide() const { return side_; }
    double GetPrice() const { return price_; }
    int GetQuantity() const { return quantity_; }

    // Mutators (if you need to change the order after creation)
    void SetClientOrderId(const std::string& client_order_id) { client_order_id_ = client_order_id; }
    void SetInstrument(const std::string& instrument) { instrument_ = instrument; }
    void SetSide(int side) { side_ = side; }
    void SetPrice(double price) { price_ = price; }
    void SetQuantity(int quantity) { quantity_ = quantity; }

private:
    std::string client_order_id_;
    std::string instrument_;
    int side_; // This could be changed to a bool or enum for better type safety.
    double price_;
    int quantity_;
};
