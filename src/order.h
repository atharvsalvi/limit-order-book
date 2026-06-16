#pragma once

class OrderCard {
	public:
		int orderID;
		double price;
		int quantity;
		int arriveTime;
		OrderCard* prev;
		OrderCard* next;
};