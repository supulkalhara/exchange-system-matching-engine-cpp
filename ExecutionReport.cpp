#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

class Order;

class ExecutionReport {
public:
    // Assuming that 'Order' is a previously defined class
    ExecutionReport(const Order& order, int status, const std::string& reason = "")
            : client_order_id_(order.GetClientOrderId()),
              order_id_("OID" + order.GetClientOrderId()),
              instrument_(order.GetInstrument()),
              side_(order.GetSide()),
              price_(order.GetPrice()),
              quantity_(order.GetQuantity()),
              status_(status),
              reason_(reason),
              transaction_time_(GetCurrentTimestamp()) {}

    // Accessors
    const std::string& GetClientOrderId() const { return client_order_id_; }
    const std::string& GetOrderId() const { return order_id_; }
    const std::string& GetInstrument() const { return instrument_; }
    int GetSide() const { return side_; }
    double GetPrice() const { return price_; }
    int GetQuantity() const { return quantity_; }
    int GetStatus() const { return status_; }
    const std::string& GetReason() const { return reason_; }
    const std::string& GetTransactionTime() const { return transaction_time_; }

private:
    std::string client_order_id_;
    std::string order_id_;
    std::string instrument_;
    int side_; // Consider using an enum as suggested earlier.
    double price_;
    int quantity_;
    int status_;
    std::string reason_;
    std::string transaction_time_;

    // Function to generate the current timestamp in the required format
    std::string GetCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto now_as_time_t = std::chrono::system_clock::to_time_t(now);
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_as_time_t), "%Y%m%d-%H%M%S");
        ss << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
        return ss.str();
    }
};
