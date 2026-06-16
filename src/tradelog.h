#pragma once
#include<vector>

class Trade {
	public:
		double price;
		char side;
		int quantity;
		int executeTime;
};

extern std::vector<Trade> tradeLog;

void printTradeLog();