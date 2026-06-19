#include "tradelog.h"
#include <iostream>
#include <algorithm>
using namespace std;

vector<Trade> tradeLog;

void printTradeLog() {
    cout << "Time\tSide\tPrice\tQuantity"<<endl;
    for(auto& trade : tradeLog) {
        cout << trade.executeTime << "\t"
             << trade.side << "\t"
             << trade.price << "\t"
             << trade.quantity << endl;
    }
}