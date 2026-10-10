#include <bits/stdc++.h>
using namespace std;
struct Order {
    string user_id;
    double price;
    double energy_amt;
    uint64_t timestamp;
    bool operator<(const Order& other) const {
        if (price == other.price) return timestamp > other.timestamp;
        return price < other.price; 
    }
};
struct AskComparator {
    bool operator()(const Order& a, const Order& b) const {
        if (a.price == b.price) return a.timestamp > b.timestamp;
        return a.price > b.price;

    }
};
class EnergyExchange {
private:
    priority_queue<Order> evbids; 
    priority_queue<Order, vector<Order>, AskComparator> soalarask;
    unordered_map<string, double> token_bal;    
    unordered_map<string, int> succ_contr;
    unordered_map<string, int> total_contr;    
    uint64_t curr_time = 0;
    const double MIN_SAFETY_MARGIN = 2.0;
    const double DEFAULT_FLOOR_PRICE = 0.10;

public:
    double total_pooled_kw = 100.0; 
    void SubmitOrder(string user_id, double price, double amount, bool is_ev_bid) {
        curr_time++;
        Order new_order = {user_id, price, amount, curr_time};        
        if (is_ev_bid) {
            evbids.push(new_order);
        } else {
            soalarask.push(new_order);
        }
    }
    void ProcessTelemetry(string user_id, double equity_share, double current_load) {
        double virtual_allocation = total_pooled_kw * equity_share;
        double net_surplus = virtual_allocation - current_load;        
        if (net_surplus > MIN_SAFETY_MARGIN) {
            SubmitOrder(user_id, DEFAULT_FLOOR_PRICE, net_surplus, false);
        }
    }
    vector<string> MatchOrders() {
        vector<string> execution_log;        
        while (!evbids.empty() && !soalarask.empty()) {
            Order topbid = evbids.top();
            Order topask = soalarask.top();
            if (topbid.price >= topask.price) {
                double matched_volume = min(topbid.energy_amt, topask.energy_amt);
                double execution_price = (topbid.price + topask.price) / 2.0;
                token_bal[topask.user_id] += (matched_volume * execution_price);
                token_bal[topbid.user_id] -= (matched_volume * execution_price);
                execution_log.push_back(topbid.user_id + " bought " + to_string(matched_volume) + 
                                        " kWh from " + topask.user_id + " at mid-market $" + to_string(execution_price));
                evbids.pop();
                soalarask.pop();                
                if (topbid.energy_amt > matched_volume) {
                    topbid.energy_amt -= matched_volume;
                    evbids.push(topbid);
                } else if (topask.energy_amt > matched_volume) {
                    topask.energy_amt -= matched_volume;
                    soalarask.push(topask);
                }
            } else {
                break; 
            }
        }
        return execution_log;
    }
    void RecordContractHistory(string user_id, bool success) {
        total_contr[user_id]++;
        if (success) {
            succ_contr[user_id]++;
        }
    }
    double CalculateCredibility(string user_id) {
        int successful_deliveries = succ_contr[user_id];
        int total_initiated = total_contr[user_id];        
        if (total_initiated == 0) return 0.0;        
        return (static_cast<double>(successful_deliveries) / total_initiated) * 100.0;
    }
};
//main fn.
int main() {
    EnergyExchange exchange;
    exchange.RecordContractHistory("Solar_User_A", true);
    exchange.RecordContractHistory("Solar_User_A", true);
    exchange.RecordContractHistory("Solar_User_A", false);
    exchange.RecordContractHistory("Solar_User_A", true);
    exchange.RecordContractHistory("Solar_User_B", true);
    exchange.RecordContractHistory("Solar_User_B", false);
    exchange.RecordContractHistory("Solar_User_B", false);
    exchange.SubmitOrder("EV_User_1", 0.18, 20.0, true);
    exchange.SubmitOrder("EV_User_2", 0.14, 10.0, true);
    exchange.ProcessTelemetry("Solar_User_A", 0.25, 5.0);
    exchange.ProcessTelemetry("Solar_User_B", 0.10, 12.0);
    vector<string> results = exchange.MatchOrders();    
    cout << "--- TRADE EXECUTION LOG ---\n";
    for (const string& trade : results) {
        cout << trade << "\n";
    }
    cout << "\n--- CREDIBILITY SCORES ---\n";
    cout << "Solar_User_A Score: " << exchange.CalculateCredibility("Solar_User_A") << "%\n";
    cout << "Solar_User_B Score: " << exchange.CalculateCredibility("Solar_User_B") << "%\n";
    cout << "New_User Score: " << exchange.CalculateCredibility("New_User") << "%\n";
    return 0;
}