#include<iostream>
#include "orderbook.h"
#include "tradelog.h"
#include <vector>
#include <cmath>
#include <numeric>
#include <chrono>
#include "publisher.h"
using namespace std;

mutex bookMutex;
mutex tradeMutex;

using TimePoint = chrono::system_clock::time_point;

OrderCard* buyHead = nullptr;
OrderCard* sellHead = nullptr;

void addSeller(OrderCard* seller, int id, double price, int quantity, TimePoint now) {

	lock_guard<std::mutex> lock(bookMutex);

	seller->prev = nullptr;
	seller->orderID = id;
	seller->price = price;
	seller->quantity = quantity;
	// seller->owner = owner;
	seller->arriveTime = now;
	if(sellHead == nullptr) {
		seller->next = nullptr;
		sellHead = seller;
	}
	else if(sellHead->next == nullptr) {
		if(sellHead->price > seller->price) {
			sellHead->prev = seller;
			seller->next = sellHead;
			sellHead = seller;
		}
		else {
			seller->prev = sellHead;
			sellHead->next = seller;
			seller->next = nullptr;
		}
	}
	else {
		if(sellHead->price > seller->price) {
			sellHead->prev = seller;
			seller->next = sellHead;
			sellHead = seller;
		}
		else {
			OrderCard* temp = sellHead;
			while(temp->next != nullptr && temp->next->price <= seller->price) {
				temp = temp->next;
			}
			if(temp->next == nullptr) {
				seller->next = nullptr;
				temp->next = seller;
				seller->prev = temp;
			}
			else {
				seller->next = temp->next;
				seller->next->prev = seller;
				seller->prev = temp;
				temp->next = seller;
			}
		}
	}
	matching_engine();
}

void addBuyer(OrderCard* buyer, int id, double price, int quantity, TimePoint now) {

	lock_guard<std::mutex> lock(bookMutex);

	buyer->prev = nullptr;
	buyer->orderID = id;
	buyer->price = price;
	buyer->quantity = quantity;
	// buyer->owner = owner;
	buyer->arriveTime = now;
	if(buyHead == nullptr) {
		buyer->next = nullptr;
		buyHead = buyer;
	}
	else if(buyHead->next == nullptr) {
		if(buyHead->price < buyer->price) {
			buyHead->prev = buyer;
			buyer->next = buyHead;
			buyHead = buyer;
		}
		else {
			buyer->prev = buyHead;
			buyHead->next = buyer;
			buyer->next = nullptr;
		}
	}
	else {
		if(buyer->price > buyHead->price) {
			buyer->next = buyHead;
			buyHead->prev = buyer;
			buyHead = buyer;
		}
		else {
			OrderCard* temp = buyHead;
			while(temp->next != nullptr && temp->next->price >= buyer->price) {
				temp = temp->next;
			}
			if(temp->next == nullptr) {
				buyer->next = nullptr;
				buyer->prev = temp;
				temp->next = buyer;
			}
			else {
				buyer->next = temp->next;
				buyer->next->prev = buyer;
				buyer->prev = temp;
				temp->next = buyer;
			}
		}
	}
	matching_engine();
}

void matching_engine() {

	while((buyHead && sellHead) && (buyHead->price >= sellHead->price)) {
		int tradeQuantity = min(buyHead->quantity, sellHead->quantity);

		buyHead->quantity -= tradeQuantity;
		sellHead->quantity -= tradeQuantity;

		if(buyHead->arriveTime > sellHead->arriveTime) {
			lock_guard lock(tradeMutex);
			tradeLog.push_back({sellHead->price, 'B', tradeQuantity, chrono::system_clock::now()});
		}
		else {
			lock_guard lock(tradeMutex);
			tradeLog.push_back({buyHead->price, 'S', tradeQuantity, chrono::system_clock::now()});
		}

		if(buyHead->quantity == 0) {
			OrderCard* buyTemp = buyHead;
			buyHead = buyHead->next;
			if(buyHead) buyHead->prev = nullptr;
			delete(buyTemp);
		}

		if(sellHead->quantity == 0) {		
			OrderCard* sellTemp = sellHead;
			sellHead = sellHead->next;
			if(sellHead) sellHead->prev = nullptr;
			delete(sellTemp);
		}

	}
}

void remove(int orderID, string type) {

	if(type == "BUY") {
		OrderCard* head = buyHead;
		if(head == nullptr) return;
		if(head->next == nullptr) {
			if (head->orderID == orderID) {
				buyHead = nullptr;
				delete head;
			}
			return;
		}
		while(head) {
			if(head->orderID == orderID) {
				if(head->prev == nullptr) {
					buyHead = buyHead->next;
					buyHead->prev = nullptr;
					head->next = nullptr;
					break;
				}
				else if(head->next == nullptr) {
					OrderCard* temp = head->prev;
					temp->next = nullptr;
					head->prev = nullptr;
					break;
				}
				else {
					OrderCard* previous = head->prev;
					OrderCard* next = head->next;
					previous->next = next;
					next->prev = previous;
					head->prev = nullptr;
					head->next = nullptr;
					break;
				}
			}
			head = head->next;
		}
		delete(head);
	}
	else {
		OrderCard* head = sellHead;
		if(head == nullptr) return;
		if(head->next == nullptr) {
			if (head->orderID == orderID) {
				sellHead = nullptr;
				delete head;
			}
			return;
		}
		while(head) {
			if(head->orderID == orderID) {
				if(head->prev == nullptr) {
					sellHead = sellHead->next;
					sellHead->prev = nullptr;
					head->next = nullptr;
					break;
				}
				else if(head->next == nullptr) {
					OrderCard* temp = head->prev;
					temp->next = nullptr;
					head->prev = nullptr;
					break;
				}
				else {
					OrderCard* previous = head->prev;
					OrderCard* next = head->next;
					previous->next = next;
					next->prev = previous;
					head->prev = nullptr;
					head->next = nullptr;
					break;
				}
			}
			head = head->next;
		}
		delete(head);
	}
}

void cancel_order(int orderID) {

	lock_guard<std::mutex> lock(bookMutex);

	OrderCard* temp = buyHead;
	bool flag = false;
	while(temp) {
		if(temp->orderID == orderID) {
			flag = true;
			remove(orderID, "BUY");
			break;
		}
		temp = temp->next;
	}
	if(flag) return;
	temp = sellHead;
	while(temp) {
		if(temp->orderID == orderID) {
			flag = true;
			remove(orderID, "SELL");
			break;
		}
		temp = temp->next;
	}
	if(flag) return;
}

