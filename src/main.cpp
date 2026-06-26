#include <iostream>
#include <algorithm>
#include "orderbook.h"
#include "tradelog.h"
#include "pnltracker.h"
#include "data/parser.h"
using namespace std;

int t = 0;

int main() {
	// int oid = 1;

	// // With matching_engine
	// auto B = [&](double price, int quantity){ addBuyer(new OrderCard(), oid++, price, quantity); t++; matching_engine(); };
	// auto S = [&](double price, int quantity){ addSeller(new OrderCard(), oid++, price, quantity); t++; matching_engine(); };

	// // Without matching_engine
	// // auto B = [&](float p, int q){ addBuyer(new OrderCard(), oid++, p, q); };
	// // auto S = [&](float p, int q){ addSeller(new OrderCard(), oid++, p, q); };

	// // Setup initial book
	// B(100, 10);
	// B(98, 5);
	// B(100,8);
	// S(102, 4);
	// S(99, 8);
	// S(99,10);
	// S(98, 15);
	// B(103, 5);
	// B(98, 3);
	// B(95, 4);

	// // Decision signal = strategy_interface();
	// cout<<endl;

	// // Final book values
	// for(OrderCard* temp = buyHead; temp; temp = temp->next) cout << "BUY id=" << temp->orderID << " price=" << temp->price << " qty=" << temp->quantity << "\n";
	// for(OrderCard* temp = sellHead; temp; temp = temp->next)cout << "SELL id=" << temp->orderID << " price=" << temp->price << " qty=" << temp->quantity << "\n";

	// printTradeLog();

	// // cout << signal <<endl;

	// double last_price = tradeLog.back().price;
	// tracker.processFill();  // or loop processFill over tradeLog
	// tracker.markToMarket(last_price);  // pass last traded price

	// cout << "Net Position: " << tracker.getNetPosition() << endl;
	// cout << "Avg Entry: " << tracker.getAvgEntryPrice() << endl;
	// cout << "Realized P&L: " << tracker.getRealizedPnL() << endl;
	// cout << "Unrealized P&L: " << tracker.getUnrealizedPnL() << endl;
	// cout << "Total P&L: " << tracker.getTotalPnL() << endl;

	// cout << endl;

	// // cancel_order(1);
	// // cancel_order(10);
	// // cancel_order(8);

	// // cancel_order(5);
	// // cancel_order(11);
	// // cancel_order(4);

	// // for(OrderCard* t = buyHead; t; t = t->next) cout << "BUY id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";
	// // for(OrderCard* t = sellHead; t; t = t->next)cout << "SELL id=" << t->orderID << " price=" << t->price << " qty=" << t->quantity << "\n";

	auto candles = loadCSV(DATA_PATH);

	// Seed with first candle so strategy has something to work with
	OrderCard* b = new OrderCard();
	OrderCard* s = new OrderCard();
	addBuyer(b, t++, candles[0].high, 1);
	addSeller(s, t++, candles[0].low, 1);
	matching_engine();
	tracker.processFill();

	// Now run the loop from candle 1 onwards
	for(size_t i = 1; i < candles.size(); i++) {
		auto& c = candles[i];
		t++;

		addSeller(new OrderCard(), t, c.high, 1);
		addBuyer(new OrderCard(),  t, c.low,  1);

		Decision d = strategy_interface();
		
		// Only trade if position is within limits
		if(d == BUY && tracker.getNetPosition() < 10) {
			addBuyer(new OrderCard(), t, c.high, 1);
		} else if(d == SELL && tracker.getNetPosition() > -10) {
			addSeller(new OrderCard(), t, c.low, 1);
		}

		matching_engine();
		tracker.processFill();
		tracker.markToMarket(c.close);
	}

	printTradeLog();
	cout << "\nNet Position:    " << tracker.getNetPosition() << endl;
	cout << "Avg Entry Price: " << tracker.getAvgEntryPrice() << endl;
	cout << "Realized P&L:    " << tracker.getRealizedPnL() << endl;
	cout << "Unrealized P&L:  " << tracker.getUnrealizedPnL() << endl;
	cout << "Total P&L:       " << tracker.getTotalPnL() << endl;

}
