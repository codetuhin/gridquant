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
    double total_pooled_kw = 0.0;
    uint64_t curr_time = 0;
public:
    void ProcessTelemetry(string user_id, double equity_share, double current_load, double initial_funds){
        total_pooled_kw += equity_share;
        token_bal[user_id] += initial_funds;
    }
    void SubmitOrder(string user_id, double price, double amt, bool is_evbid){    
        curr_time++; 
        Order new_order = {user_id, price, amt, curr_time};
        if (is_evbid){
            evbids.push(new_order);
        }
        else{        
            solarask.push(new_order);
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
            double trad_vol = min(bestbid.energy_amt, bestask.energy_amt);
            double total_cost = trad_vol * bestask.price;
            token_bal[bestbid.user_id] -= total_cost;
            token_bal[bestask.user_id] += total_cost;
            execution_log.push_back(bestbid.user_id + " bought " + to_string(trad_vol) +
                                    " kWh from " + bestask.user_id + " for $" + to_string(total_cost));
            bestbid.energy_amt -= trad_vol;
            bestask.energy_amt -= trad_vol;

            if (bestbid.energy_amt > 0) evbids.push(bestbid);
            if (bestask.energy_amt > 0) solarask.push(bestask);
        }
        return execution_log;
    }
    double CalculateCredibility(string user_id){
            if (token_bal.count(user_id) && token_bal[user_id] > 0){        
            return 99.9;
        }
        return 50.0;
    }
    void PrintLedger(){
        cout << "\n--- CURRENT LEDGER BALANCES ---\n";
        for (const auto &pair : token_bal){
            cout << pair.first << ": $" << pair.second << "\n";
        }
    }
};
// main fn.
int main(){
    EnergyExchange exchange;
    exchange.ProcessTelemetry("EV_User_1", 0.0, 10.0, 50.00);
    exchange.ProcessTelemetry("EV_User_2", 0.0, 5.0, 20.00);
    exchange.ProcessTelemetry("Solar_User_A", 15.0, 2.0, 5.00);
    exchange.ProcessTelemetry("Solar_User_B", 10.0, 1.0, 10.00);
    exchange.SubmitOrder("EV_User_1", 0.15, 10.0, true);
    exchange.SubmitOrder("EV_User_2", 0.18, 5.0, true);
    exchange.SubmitOrder("Solar_User_A", 0.12, 15.0, false);
    exchange.SubmitOrder("Solar_User_B", 0.14, 5.0, false);
    vector<string> results = exchange.MatchOrders();
    cout << "--- TRADE EXECUTION LOG ---\n";
    for (const string &trade : results){
        cout << trade << "\n";
    }
    exchange.PrintLedger();
    cout << "\nCredibility for EV_User_1: " << exchange.CalculateCredibility("EV_User_1") << "\n";
    return 0;
}