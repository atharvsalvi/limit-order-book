#pragma once
#include <vector>
#include <chrono>
#include <mutex>
using namespace std;

using TimePoint = chrono::system_clock::time_point;

class Trade {
	public:
		double price;
		char side;
		int quantity;
		TimePoint executeTime;
};

extern std::vector<Trade> tradeLog;
extern std::mutex tradeMutex;

void printTradeLog();