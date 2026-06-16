#include <iostream>
#include "orderbook.h"
#include "tradelog.h"
using namespace std;

int t = 0;

int main() {
	int oid = 1;

	// With matching_engine
	auto B = [&](double p, int q){ addBuyer(new OrderCard(), oid++, p, q); t++; matching_engine(); };
	auto S = [&](double p, int q){ addSeller(new OrderCard(), oid++, p, q); t++; matching_engine(); };

	// Without matching_engine
	// auto B = [&](float p, int q){ addBuyer(new OrderCard(), oid++, p, q); };
	// auto S = [&](float p, int q){ addSeller(new OrderCard(), oid++, p, q); };

	// Setup initial book
	B(100, 10);
	B(98, 5);
	B(100,8);
	S(102, 4);
	S(99, 8);
	S(99,10);
	S(98, 15);
	B(103, 5);
	B(98, 3);
	B(95, 4);

	// Final book values
	for(OrderCard* temp = buyHead; temp; temp = temp->next) cout << "BUY id=" << temp->orderID << " price=" << temp->price << " qty=" << temp->quantity << "\n";
	for(OrderCard* temp = sellHead; temp; temp = temp->next)cout << "SELL id=" << temp->orderID << " price=" << temp->price << " qty=" << temp->quantity << "\n";

	printTradeLog();

	cout << endl;

	// cancel_order(1);
	// cancel_order(10);
	// cancel_order(8);

	// cancel_order(5);
	// cancel_order(11);
	// cancel_order(4);

	// for(OrderCard* t = buyHead; t; t = t->next) cout << "BUY id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";
	// for(OrderCard* t = sellHead; t; t = t->next)cout << "SELL id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";

}
