#include <iostream>
using namespace std;

class OrderCard {
	public:
		int orderID;
		string orderType;
		float price;
		int quantity;
		OrderCard* prev;
		OrderCard* next;
};

OrderCard* buyHead = NULL;
OrderCard* sellHead = NULL;

void addSeller(OrderCard* seller, int id, float price, int quantity) {
	seller->prev = NULL;
	seller->orderID = id;
	seller->price = price;
	seller->orderType = "SELL";
	seller->quantity = quantity;
	if(sellHead == NULL) {
		seller->next = NULL;
		sellHead = seller;
	}
	else if(sellHead->next == NULL) {
		if(sellHead->price >= seller->price) {
			sellHead->prev = seller;
			seller->next = sellHead;
			sellHead = seller;
		}
		else {
			seller->prev = sellHead;
			sellHead->next = seller;
			seller->next = NULL;
		}
	}
	else {
		if(sellHead->price > seller->price) {
			sellHead->prev = seller;
			seller->next = sellHead;
			sellHead = seller;
			return;
		}
		OrderCard* temp = sellHead;
		while(temp->next != NULL && temp->next->price <= seller->price) {
			temp = temp->next;
		}
		if(temp->next == NULL) {
			seller->next = NULL;
			temp->next = seller;
			seller->prev = temp;
			return;
		}
		seller->next = temp->next;
		seller->next->prev = seller;
		seller->prev = temp;
		temp->next = seller;
	}
}

void addBuyer(OrderCard* buyer, int id, float price, int quantity) {
	buyer->prev = NULL;
	buyer->orderID = id;
	buyer->price = price;
	buyer->orderType = "BUY";
	buyer->quantity = quantity;
	if(buyHead == NULL) {
		buyer->next = NULL;
		buyHead = buyer;
	}
	else if(buyHead->next == NULL) {
		if(buyHead->price <= buyer->price) {
			buyHead->prev = buyer;
			buyer->next = buyHead;
			buyHead = buyer;
		}
		else {
			buyer->prev = buyHead;
			buyHead->next = buyer;
			buyer->next = NULL;
		}
	}
	else {
		if(buyer->price > buyHead->price) {
			buyer->next = buyHead;
			buyHead->prev = buyer;
			buyHead = buyer;
			return;
		}
		OrderCard* temp = buyHead;
		while(temp->next != NULL && temp->next->price >= buyer->price) {
			temp = temp->next;
		}
		if(temp->next == NULL) {
			buyer->next = NULL;
			buyer->prev = temp;
			temp->next = buyer;
			return;
		}
		buyer->next = temp->next;
		buyer->next->prev = buyer;
		buyer->prev = temp;
		temp->next = buyer;
	}
}

void matching_engine() {
	while((buyHead != NULL && sellHead != NULL) && (buyHead->price >= sellHead->price)) {
		int tradeQuantity = min(buyHead->quantity, sellHead->quantity);

		buyHead->quantity -= tradeQuantity;
		sellHead->quantity -= tradeQuantity;

		if(buyHead->quantity == 0) {
			OrderCard* buyTemp = buyHead;
			buyHead = buyHead->next;
			if(buyHead) buyHead->prev = NULL;
			delete(buyTemp);
		}

		if(sellHead->quantity == 0) {		
			OrderCard* sellTemp = sellHead;
			sellHead = sellHead->next;
			if(sellHead) sellHead->prev = NULL;
			delete(sellTemp);
		}
	}
}

int main() {
	int oid = 1;
	auto B = [&](float p, int q){ addBuyer(new OrderCard(), oid++, p, q); matching_engine(); };
	auto S = [&](float p, int q){ addSeller(new OrderCard(), oid++, p, q); matching_engine(); };

	// Setup initial book
	B(100, 10); // id1
	B(98, 5);   // id2

	S(102, 4);  // id3 -- no cross, sits in book

	// Test 1: Partial fill of resting buy
	S(99, 8);   // id4
	// 99 <= 100 -> matches id1 at price 100
	// trade qty = min(8,10) = 8
	// id1 remaining qty = 2
	// id4 remaining qty = 0 -> fully filled, not added to book
	// Next check: id4 done, stop.

	// Test 2: Full fill of resting buy, partial spillover, then no-cross stop
	S(98, 15);  // id5
	// id1 (qty=2, price=100): 98<=100 -> match, trade=2, id1 removed, id5 remaining=13
	// next best buy = id2 (price=98): 98<=98 -> match, trade=min(13,5)=5, id2 removed, id5 remaining=8
	// no more buys -> id5 enters SELL book with qty=8, price=98

	// Test 3: Incoming BUY crosses existing SELL
	B(103, 5);  // id6
	// best sell = id5 (price=98, qty=8): 103>=98 -> match, trade=min(5,8)=5
	// id6 fully filled (remaining=0), id5 remaining=3
	// stop, id6 not added to book

	// Test 4: Incoming BUY exactly matches and fully clears a sell, then doesn't cross next
	B(98, 3);   // id7
	// best sell = id5 (price=98, qty=3): 98>=98 -> match, trade=3, id5 removed
	// id7 remaining=0, fully filled, stop

	// Test 5: No cross at all
	B(95, 4);   // id8
	// best sell = id3 (price=102): 95>=102? No -> no match, id8 enters BUY book

	for(OrderCard* t = buyHead; t; t = t->next)
		cout << "BUY id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";
	for(OrderCard* t = sellHead; t; t = t->next)
		cout << "SELL id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";
}
