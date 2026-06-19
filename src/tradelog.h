#pragma once
#include <vector>
using namespace std;

class Trade {
	public:
		double price;
		char side;
		int quantity;
		int executeTime;
};

extern std::vector<Trade> tradeLog;

void printTradeLog();