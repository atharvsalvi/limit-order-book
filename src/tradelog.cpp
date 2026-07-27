#include "tradelog.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <ctime>
using namespace std;

vector<Trade> tradeLog;

void printTradeLog() {
    cout << "Time\tSide\tPrice\tQuantity"<<endl;
    for(auto& trade : tradeLog) {

        time_t tt = chrono::system_clock::to_time_t(trade.executeTime);

        cout << put_time(localtime(&tt), "%Y-%m-%d %H:%M:%S") << "\t"
             << trade.side << "\t"
             << trade.price << "\t"
             << trade.quantity << endl;
    }
}