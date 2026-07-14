#include <iostream>
#include <algorithm>
#include <chrono>
#include "orderbook.h"
#include "tradelog.h"
#include "data/parser.h"
// #include "strategy.h"
// #include "pnltracker.h"
using namespace std;

int t = 0;
// MeanReversionStrategy strategy;

int main() {

	auto candles = loadCSV(DATA_PATH);

	auto start = std::chrono::high_resolution_clock::now();

	for(size_t i = 0; i < candles.size(); i++) {
		auto& c = candles[i];

		addBuyer(new OrderCard(),  t++, c.open, 1);
		addSeller(new OrderCard(), t++, c.close, 1);

		matching_engine();
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	for(OrderCard* t = buyHead; t; t = t->next) cout << "BUY id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << "\n";
	for(OrderCard* t = sellHead; t; t = t->next) cout << "SELL id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << "\n";

	for(auto it : tradeLog) {
		cout << "Side=" << it.side << " Price= "<< it.price << "\n";
	}

	printTradeLog();

	cout << "\nCandles processed: " << candles.size() - 1 << endl;
	cout << "Total time:        " << duration.count() << " us" << endl;
	cout << "Avg latency/tick:  " << (double)duration.count() / (candles.size() - 1) << " us" << endl;

}
