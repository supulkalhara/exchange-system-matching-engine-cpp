#include <string>

class Trade {
public:
    // Constructor
    Trade(const std::string& order_id,
          const std::string& client_order_id,
          const std::string& instrument,
          int side,
          double price,
          int quantity,
          int status,
          const std::string& reason,
          const std::string& transaction_time)
            : order_id_(order_id),
              client_order_id_(client_order_id),
              instrument_(instrument),
              side_(side),
              price_(price),
              quantity_(quantity),
              status_(status),
              reason_(reason),
              transaction_time_(transaction_time) {}

    // Accessors
    const std::string& GetOrderId() const { return order_id_; }
    const std::string& GetClientOrderId() const { return client_order_id_; }
    const std::string& GetInstrument() const { return instrument_; }
    int GetSide() const { return side_; }
    double GetPrice() const { return price_; }
    int GetQuantity() const { return quantity_; }
    int GetStatus() const { return status_; }
    const std::string& GetReason() const { return reason_; }
    const std::string& GetTransactionTime() const { return transaction_time_; }

private:
    std::string order_id_;
    std::string client_order_id_;
    std::string instrument_;
    int side_;
    double price_;
    int quantity_;
    int status_;
    std::string reason_;
    std::string transaction_time_;
};
