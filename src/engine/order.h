#pragma once
#include<chrono>

#include "tradelog/tradelog.h"
#include "logger/logger.h"

using TimePoint = std::chrono::system_clock::time_point;

// enum Owner { STRATEGY, LIQUIDITY };

class OrderCard {
	public:
		int orderID;
		double price;
		int quantity;
		TimePoint arriveTime;
		// Owner owner;
		OrderCard* prev;
		OrderCard* next;
};

enum Decision {
	BUY,
	SELL,
	HOLD
};