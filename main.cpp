#include <fstream>
#include <sstream>
#include <vector>
#include <map>


class MatchingEngine;

class ExecutionReport;

class Order;

class Main {
public:
    void ProcessOrders(const std::string& csv_file_path) {
        MatchingEngine matching_engine;
        std::vector<ExecutionReport> execution_reports;

        std::ifstream file(csv_file_path);
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream sstream(line);
            std::string cell;
            std::vector<std::string> row;
            while (std::getline(sstream, cell, ',')) {
                row.push_back(cell);
            }
            if (row.size() < 5) continue; // Skip invalid rows

            // Create an Order object from the row
            Order order(row[0], row[1], row[2], std::stod(row[3]), std::stoi(row[4]));
            ExecutionReport report = ProcessSingleOrder(order, matching_engine);
            execution_reports.push_back(report);
        }

        // Write execution reports to CSV
        WriteExecutionReportsToCsv("execution_rep.csv", execution_reports);
    }

private:
    ExecutionReport ProcessSingleOrder(Order& order, MatchingEngine& matching_engine) {
        // Basic validation before processing
        if (order.GetQuantity() % 10 != 0 || order.GetQuantity() < 10 || order.GetQuantity() > 1000) {
            return ExecutionReport(order, 1, "Invalid Quantity");
        } else if (order.GetInstrument() != "Rose" && order.GetInstrument() != "Lavender" &&
                   order.GetInstrument() != "Lotus" && order.GetInstrument() != "Tulip" &&
                   order.GetInstrument() != "Orchid") {
            return ExecutionReport(order, 1, "Invalid Instrument");
        } else if (order.GetPrice() <= 0.0) {
            return ExecutionReport(order, 1, "Invalid Price");
        } else {
            // Process the order in the matching engine
            matching_engine.ProcessOrder(order);
            // Assume the order is accepted for now
            return ExecutionReport(order, 0);
        }
    }

    void WriteExecutionReportsToCsv(const std::string& file_path, const std::vector<ExecutionReport>& reports) {
        std::ofstream out_file(file_path);
        // Write header
        out_file << "Client Order ID,Order ID,Instrument,Side,Price,Quantity,Status,Reason,Transaction Time\n";
        for (const auto& report : reports) {
            out_file << report.ToCsvRow() << std::endl;
        }
    }
};

int main() {
    Main app;
    app.ProcessOrders("orders.csv");
    return 0;
}