#include <iostream>
#include <algorithm>
#include <chrono>
#include "orderbook.h"
#include "tradelog.h"
// #include "data/parser.h"
#include "logger/logger.h"
// #include "strategy.h"
// #include "pnltracker.h"
using namespace std;

int t = 0;
// MeanReversionStrategy strategy;

struct ManualOrder {
    char side;
    double price;
    int qty;
};

vector<ManualOrder> orders = {
    {'B', 182.50, 1},
    {'S', 183.00, 1},
    {'B', 184.00, 1},
    {'S', 182.00, 1},
    // add more test cases here
};

int main() {

	OrderLogger logger("trade_log.txt");

	if (logger.exists()) {
		std::cout << "Found existing trade_log.txt — replaying to rebuild book state...\n";
		t = logger.replay();
		std::cout << "Replay complete. Resuming from t=" << t << "\n";
	}
	logger.openForAppend();


	// auto candles = loadCSV(DATA_PATH);

	// auto start = std::chrono::high_resolution_clock::now();

	int choice;

	do {

		cout << "Select an option\n";
		cout << "1. Buy\n";
		cout << "2. Sell\n";
		cout << "3. Print Buy and Sell orders\n";
		cout << "4. Print tradelog\n";
		cout << "5. Exit\n";

		cin >> choice;

		switch(choice) {
			case 1 : {
				double price;
				int quantity;
				cout << "Enter quantity\n";
				cin >> quantity;
				cout << "Enter price\n";
				cin >> price;
				logger.logOrder('B', t, price, quantity);
				addBuyer(new OrderCard(), t++, price, quantity);
				matching_engine();
				break;
			}
			case 2 : {
				double price;
				int quantity;
				cout << "Enter quantity\n";
				cin >> quantity;
				cout << "Enter price\n";
				cin >> price;
				logger.logOrder('S', t, price, quantity);
				addSeller(new OrderCard(), t++, price, quantity);
				matching_engine();
				break;
			}
			case 3 : {
				for(OrderCard* t = buyHead; t; t = t->next) cout << "BUY id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << "\n";
				for(OrderCard* t = sellHead; t; t = t->next) cout << "SELL id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << "\n";
				break;
			}
			case 4 : {
				printTradeLog();
				break;
			}
		}
	}while(choice != 5);

	// for (auto& o : orders) {
	// 	logger.logOrder(o.side, t, o.price, o.qty);
	// 	if (o.side == 'B')
	// 		addBuyer(new OrderCard(), t++, o.price, o.qty);
	// 	else
	// 		addSeller(new OrderCard(), t++, o.price, o.qty);

	// 	matching_engine();
	// }

	// auto end = std::chrono::high_resolution_clock::now();
	// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	// for(OrderCard* t = buyHead; t; t = t->next) cout << "BUY id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << "\n";
	// for(OrderCard* t = sellHead; t; t = t->next) cout << "SELL id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << "\n";

	// // for(auto it : tradeLog) {
	// // 	cout << "Side=" << it.side << " Price= "<< it.price << "\n";
	// // }

	// printTradeLog();

	// cout << "\nCandles processed: " << orders.size() - 1 << endl;
	// cout << "Total time:        " << duration.count() << " us" << endl;
	// cout << "Avg latency/tick:  " << (double)duration.count() / (orders.size() - 1) << " us" << endl;

}
