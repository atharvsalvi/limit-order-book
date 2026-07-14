#pragma once

// enum Owner { STRATEGY, LIQUIDITY };

class OrderCard {
	public:
		int orderID;
		double price;
		int quantity;
		int arriveTime;
		// Owner owner;
		OrderCard* prev;
		OrderCard* next;
};

enum Decision {
	BUY,
	SELL,
	HOLD
};