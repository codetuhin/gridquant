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
struct AskComparator
{
    bool operator()(const Order &a, const Order &b) const{    
        if (a.price == b.price) return a.timestamp > b.timestamp;
        return a.price > b.price;

    }
};
class Solution
{
public:
    vector<string> matchOrders(vector<Order> &bids, vector<Order> &asks){    
        priority_queue<Order> bidqueue;
        priority_queue<Order, vector<Order>, AskComparator> askqueue;
        for (const auto &bid : bids)
            bidqueue.push(bid);
        for (const auto &ask : asks)
            askqueue.push(ask);
        vector<string> execution_log;
        
        while (!bidqueue.empty() && !askqueue.empty()){        
            Order bestbid = bidqueue.top();
            Order bestask = askqueue.top();           
            if (bestbid.price < bestask.price){            
                break;
            }            
            bidqueue.pop();
            askqueue.pop();        
            double trad_vol = min(bestbid.energy_amt, bestask.energy_amt);           
            execution_log.push_back(bestbid.user_id + " bought " + to_string(trad_vol) +
                                    " kWh from " + bestask.user_id + " at $" + to_string(bestask.price));           
            bestbid.energy_amt -= trad_vol;
            bestask.energy_amt -= trad_vol;            
            if (bestbid.energy_amt > 0) bidqueue.push(bestbid);            
            if (bestask.energy_amt > 0) askqueue.push(bestask);            
        }
        return execution_log;
    }
};

// main fn.
int main()
{
    vector<Order> bids = {
        {"EV_User_1", 0.15, 10.0, 100},
        {"EV_User_2", 0.18, 5.0, 101},
        {"EV_User_3", 0.16, 8.0, 102}};
    vector<Order> asks = {
        {"Solar_User_A", 0.12, 15.0, 100},
        {"Solar_User_B", 0.14, 5.0, 101},
        {"Solar_User_C", 0.13, 10.0, 102}};
    Solution sol;
    vector<string> results = sol.matchOrders(bids, asks);
    cout << "--- TRADE EXECUTION LOG ---\n";
    for (const string &trade : results){    
        cout << trade << "\n";
    }

    return 0;
}