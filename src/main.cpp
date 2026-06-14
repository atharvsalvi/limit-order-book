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
		if(sellHead->price > seller->price) {
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
		if(buyHead->price < buyer->price) {
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
	
	// Testing
	// if(buyHead != NULL) cout << "BUY id=" << buyHead->orderID << " price=" << buyHead->price << " qty=" << buyHead->quantity << "\n";
	// if(sellHead!= NULL) cout << "SELL id=" << sellHead->orderID << " price=" << sellHead->price << " qty=" << sellHead->quantity << "\n";
	// cout << endl;

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

		// Testing
		// cout << "After matching engine"<<endl;
		// for(OrderCard* t = buyHead; t; t = t->next) cout << "BUY id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";
		// for(OrderCard* t = sellHead; t; t = t->next) cout << "SELL id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";

	}
}

void remove(int orderID, string type) {
	if(type == "BUY") {
		OrderCard* head = buyHead;
		if(head->next == NULL) {
			buyHead = NULL;
			delete(head);
			return;
		}
		while(head) {
			if(head->orderID == orderID) {
				if(head->prev == NULL) {
					buyHead = buyHead->next;
					buyHead->prev = NULL;
					head->next = NULL;
					break;
				}
				else if(head->next == NULL) {
					OrderCard* temp = head->prev;
					temp->next = NULL;
					head->prev = NULL;
					break;
				}
				else {
					OrderCard* previous = head->prev;
					OrderCard* next = head->next;
					previous->next = next;
					next->prev = previous;
					head->prev = NULL;
					head->next = NULL;
					break;
				}
			}
			head = head->next;
		}
		delete(head);
	}
	else {
		OrderCard* head = sellHead;
		if(head->next == NULL) {
			sellHead = NULL;
			delete(head);
			return;
		}
		while(head) {
			if(head->orderID == orderID) {
				if(head->prev == NULL) {
					sellHead = sellHead->next;
					sellHead->prev = NULL;
					head->next = NULL;
					break;
				}
				else if(head->next == NULL) {
					OrderCard* temp = head->prev;
					temp->next = NULL;
					head->prev = NULL;
					break;
				}
				else {
					OrderCard* previous = head->prev;
					OrderCard* next = head->next;
					previous->next = next;
					next->prev = previous;
					head->prev = NULL;
					head->next = NULL;
					break;
				}
			}
			head = head->next;
		}
		delete(head);
	}
}

void cancel_order(int orderID) {
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
	cout << "Order ID '" <<orderID<< "' does not exist!" <<endl;
	cout << endl;
}

int main() {
	int oid = 1;

	// With matching_engine
	// auto B = [&](float p, int q){ addBuyer(new OrderCard(), oid++, p, q); matching_engine(); cout << endl;};
	// auto S = [&](float p, int q){ addSeller(new OrderCard(), oid++, p, q); matching_engine(); cout << endl;};

	// Without matching_engine
	auto B = [&](float p, int q){ addBuyer(new OrderCard(), oid++, p, q); };
	auto S = [&](float p, int q){ addSeller(new OrderCard(), oid++, p, q); };

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
	for(OrderCard* t = buyHead; t; t = t->next) cout << "BUY id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";
	for(OrderCard* t = sellHead; t; t = t->next)cout << "SELL id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";

	cout << endl;

	cancel_order(1);
	cancel_order(10);
	cancel_order(8);

	cancel_order(5);
	cancel_order(11);
	// cancel_order(4);

	for(OrderCard* t = buyHead; t; t = t->next) cout << "BUY id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";
	for(OrderCard* t = sellHead; t; t = t->next)cout << "SELL id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";

}
