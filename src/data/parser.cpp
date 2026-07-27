#include <fstream>
#include <sstream>
#include <vector>
#include "parser.h"
#include "orderbook.h"
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

vector<Candle> run_simulation(const string& path) {
    vector<Candle> data;
    ifstream file(path);
    string line;
    // getline(file, line);
    while(getline(file, line)) {

        line = line.substr(1, line.size() - 2);

        stringstream ss(line);
        string cmd;

        getline(ss,cmd,',');

        if (cmd == "ADD") {
            string idStr, side, qtyStr, priceStr;
            getline(ss, idStr, ',');
            getline(ss, side, ',');
            getline(ss, qtyStr, ',');
            getline(ss, priceStr, ',');
            long id = stol(idStr);
            long qty = stol(qtyStr);
            double price = stod(priceStr);
            if (side == "BUY") addBuyer(new OrderCard, id, price, qty, chrono::system_clock::now());
            else addSeller(new OrderCard, id, price, qty, chrono::system_clock::now());
        } else if (cmd == "CANCEL") {
            string idStr; getline(ss, idStr, ',');
            cancel_order(stol(idStr));
        }

        this_thread::sleep_for(std::chrono::seconds(1));

        // cout << cmd <<endl;
    }
    return data;
}