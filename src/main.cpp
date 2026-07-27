#include <iostream>
#include <algorithm>
#include <chrono>
#include "orderbook.h"
#include "tradelog.h"
#include "data/parser.h"
#include "logger/logger.h"
// #include "strategy.h"
// #include "pnltracker.h"
using namespace std;

int t = 0;

int main() {   

	OrderLogger logger("trade_log.txt");

	if (logger.exists()) {
		std::cout << "Found existing trade_log.txt — replaying to rebuild book state...\n";
		vector<RecoveredOrder> temp = logger.replay();

		for(auto it : temp) {
			if(it.side == 'S') {
				addSeller(new OrderCard(), t++, it.price, it.qty, it.arrivalTime);
			}
			else {
				addBuyer(new OrderCard(), t++, it.price, it.qty, it.arrivalTime);
			}
			matching_engine();
		}

		std::cout << "Replay complete. Resuming from t=" << t << "\n";
	}
	logger.openForAppend();

	// int choice;
	// do {

	// 	cout << "Select an option\n";
	// 	cout << "1. Buy\n";
	// 	cout << "2. Sell\n";
	// 	cout << "3. Print Buy and Sell orders\n";
	// 	cout << "4. Print tradelog\n";
	// 	cout << "5. Exit\n";

	// 	cin >> choice;

	// 	switch(choice) {
	// 		case 1 : {
	// 			double price;
	// 			int quantity;
	// 			cout << "Enter quantity\n";
	// 			cin >> quantity;
	// 			cout << "Enter price\n";
	// 			cin >> price;
	// 			logger.logOrder('B', t, price, quantity);
	// 			addBuyer(new OrderCard(), t++, price, quantity);
	// 			matching_engine();
	// 			break;
	// 		}
	// 		case 2 : {
	// 			double price;
	// 			int quantity;
	// 			cout << "Enter quantity\n";
	// 			cin >> quantity;
	// 			cout << "Enter price\n";
	// 			cin >> price;
	// 			logger.logOrder('S', t, price, quantity);
	// 			addSeller(new OrderCard(), t++, price, quantity);
	// 			matching_engine();
	// 			break;
	// 		}
	// 		case 3 : {
	// 			for(OrderCard* t = buyHead; t; t = t->next) cout << "BUY id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << "\n";
	// 			for(OrderCard* t = sellHead; t; t = t->next) cout << "SELL id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << "\n";
	// 			break;
	// 		}
	// 		case 4 : {
	// 			printTradeLog();
	// 			break;
	// 		}
	// 		case 5 : {
	// 			cout << "Shutting down... cleaning up memory.\n";

	// 			OrderCard* current = buyHead;
	// 			while (current != nullptr) {
	// 				OrderCard* nextCard = current->next;
	// 				delete current;
	// 				current = nextCard;
	// 			}

	// 			current = sellHead;
	// 			while (current != nullptr) {
	// 				OrderCard* nextCard = current->next;
	// 				delete current;
	// 				current = nextCard;
	// 			}
	// 			break;
	// 		}
	// 	}
	// }while(choice != 5);

	run_simulation(DATA_PATH);

	for(OrderCard* t = buyHead; t; t = t->next) {

		time_t tt = chrono::system_clock::to_time_t(t->arriveTime);
		cout << "BUY id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << " arrive time=" << put_time(localtime(&tt), "%Y-%m-%d %H:%M:%S") << "\n";
		
	}
	for(OrderCard* t = sellHead; t; t = t->next) {

		time_t tt = chrono::system_clock::to_time_t(t->arriveTime);
		cout << "SELL id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << " arrive time=" << put_time(localtime(&tt), "%Y-%m-%d %H:%M:%S") << "\n";

	}

	printTradeLog();

}
