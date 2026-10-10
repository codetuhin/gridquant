#include <bits/stdc++.h>
using namespace std;
struct Order{
    string user_id;
    double price;
    double energy_amt;
    uint64_t timestamp;
    bool operator<(const Order &other) const{
        if (price == other.price) return timestamp > other.timestamp;
        return price < other.price;
    }
};
struct AskComparator{
    bool operator()(const Order &a, const Order &b) const{
        if (a.price == b.price) return a.timestamp > b.timestamp;
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
    void SubmitOrder(string user_id, double price, double amt, bool is_evbid){
        curr_time++;
        Order new_order = {user_id, price, amt, curr_time};
        if (is_evbid) evbids.push(new_order);
        else
        {
            solarask.push(new_order);
        }
    }

    void ProcessTelemetry(string user_id, double equity_share, double curr_load){
        double virtual_allocation = total_pooled_kw * equity_share;
        double net_surplus = virtual_allocation - curr_load;
        if (net_surplus > MIN_SAFETY_MARGIN){
            SubmitOrder(user_id, DEFAULT_FLOOR_PRICE, net_surplus, false);
        }
    }

    vector<string> MatchOrders(){
        vector<string> execution_log;
        while (!evbids.empty() && !solarask.empty()){
            Order top_bid = evbids.top();
            Order topask = solarask.top();
            if (top_bid.price >= topask.price){
                double matched_volume = min(top_bid.energy_amt, topask.energy_amt);
                double execution_price = (top_bid.price + topask.price) / 2.0;
                token_bal[topask.user_id] += (matched_volume * execution_price);
                token_bal[top_bid.user_id] -= (matched_volume * execution_price);
                execution_log.push_back(top_bid.user_id + " bought " + to_string(matched_volume) +
                                        " kWh from " + topask.user_id + " at mid-market $" + to_string(execution_price));
                evbids.pop();
                solarask.pop();
                if (top_bid.energy_amt > matched_volume){
                    top_bid.energy_amt -= matched_volume;
                    evbids.push(top_bid);
                }
                else if (topask.energy_amt > matched_volume)
                {
                    topask.energy_amt -= matched_volume;
                    solarask.push(topask);
                }
            }
            else
            {
                break;
            }
        }
        return execution_log;
    }
};
// main fn.

int main()
{
    EnergyExchange exchange;
    exchange.SubmitOrder("EV_User_1", 0.18, 20.0, true);
    exchange.SubmitOrder("EV_User_2", 0.14, 10.0, true);
    exchange.ProcessTelemetry("Solar_User_A", 0.25, 5.0);
    exchange.ProcessTelemetry("Solar_User_B", 0.10, 12.0);
    exchange.ProcessTelemetry("Solar_User_C", 0.40, 15.0);
    vector<string> results = exchange.MatchOrders();
    for (const string &trade : results){
        cout << trade << "\n";
    }

    return 0;
}