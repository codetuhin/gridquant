#include <bits/stdc++.h>
using namespace std;
struct Order{
    string user_id;
    double price;
    double energy_amt;
    uint64_t timestamp;
    bool operator<(const Order &other) const{
        if (price == other.price)
            return timestamp > other.timestamp;
        return price < other.price;
    }
};
struct AskComparator{
    bool operator()(const Order &a, const Order &b) const{
        if (a.price == b.price)
            return a.timestamp > b.timestamp;
        return a.price > b.price;
    }
};
class EnergyExchange{
private:
    priority_queue<Order> evbids;
    priority_queue<Order, vector<Order>, AskComparator> solarask;
    unordered_map<string, double> token_bal;
    uint64_t curr_time = 0;
    const double MIN_SAFETY_MARGIN = 2.0;
    const double DEFAULT_FLOOR_PRICE = 0.10;

public:
    double total_pooled_kw = 100.0;
    void SubmitOrder(string user_id, double price, double amount, bool is_ev_bid){
        curr_time++;
        Order new_order = {user_id, price, amount, curr_time};
        if (is_ev_bid){
            evbids.push(new_order);
        }
        else{
            solarask.push(new_order);
        }
    }
    void ProcessTelemetry(string user_id, double equity_share, double current_load){
        double virtual_allocation = total_pooled_kw * equity_share;
        double net_surplus = virtual_allocation - current_load;
        if (net_surplus > MIN_SAFETY_MARGIN){
            SubmitOrder(user_id, DEFAULT_FLOOR_PRICE, net_surplus, false);
        }
    }
    vector<string> MatchOrders(){
        vector<string> execution_log;
        while (!evbids.empty() && !solarask.empty()){
            Order bestbid = evbids.top();
            Order bestask = solarask.top();
            if (bestbid.price < bestask.price){
                break;
            }
            evbids.pop();
            solarask.pop();
            double traded_volume = min(bestbid.energy_amt, bestask.energy_amt);
            double total_cost = traded_volume * bestask.price;
            token_bal[bestbid.user_id] -= total_cost;
            token_bal[bestask.user_id] += total_cost;
            execution_log.push_back(bestbid.user_id + " bought " + to_string(traded_volume) +
                                    " kWh from " + bestask.user_id + " for $" + to_string(total_cost));
            bestbid.energy_amt -= traded_volume;
            bestask.energy_amt -= traded_volume;
            if (bestbid.energy_amt > 0) evbids.push(bestbid);
            if (bestask.energy_amt > 0) solarask.push(bestask);
        }
        return execution_log;
    }
};
// main fn.

int main()
{
    EnergyExchange exchange;
    exchange.SubmitOrder("EV_User_1", 0.15, 20.0, true);
    exchange.SubmitOrder("EV_User_2", 0.12, 10.0, true);
    exchange.ProcessTelemetry("Solar_User_A", 0.25, 5.0);
    exchange.ProcessTelemetry("Solar_User_B", 0.10, 12.0);
    exchange.ProcessTelemetry("Solar_User_C", 0.40, 15.0);
    vector<string> results = exchange.MatchOrders();
    for (const string &trade : results){
        cout << trade << "\n";
    }
    return 0;
}