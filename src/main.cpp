#include <iostream>
#include <chrono>
#include <thread>

#include "engine/orderbook.h"
#include "tradelog/tradelog.h"

#include "pipeline/parser.h"
#include "pipeline/simulation.h"

#include "logger/logger.h"

#include "ui/dashboard.h"
#include "ui/tui.h"

using namespace std;

int t = 0;
bool isReplaying = true;

void run_simulation(const std::string& path) {

    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: "
                  << path << '\n';
        return;
    }

    CSVParser parser;
    Simulation simulation;

    std::string line;

    while (std::getline(file, line)) {

        auto event = parser.parse_line(line);

        if (!event) {
            std::cerr << "Skipping invalid line: "
                      << line << '\n';
            continue;
        }

        simulation.process(*event);

		this_thread::sleep_for(std::chrono::seconds(2));

    }
}

int main(int argc, char* argv[]) { 

	bool useTUI = false;

	if (argc > 1 && std::string(argv[1]) == "--tui") {
        useTUI = true;
    }

	if(useTUI) {
		if (logger.exists()) {
			std::cout << "Found existing trade_log.txt — replaying to rebuild book state...\n";
			vector<RecoveredOrder> temp = logger.replay();

			for(auto it : temp) {
				if(it.side == 'S') {
					addSeller(new OrderCard(), t++, it.price, it.qty, it.arrivalTime);
				}
				else if(it.side == 'B') {
					addBuyer(new OrderCard(), t++, it.price, it.qty, it.arrivalTime);
				}
				else {
					cancel_order(it.id);
				}
				matching_engine();
			}

			std::cout << "Replay complete. Resuming from t=" << t << "\n";
		}
		logger.openForAppend();
		isReplaying = false;
	}	

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

	std::thread simulationThread([] {
		run_simulation(DATA_PATH);
	});

	if(useTUI) {
		startTUI();
	}

	if (simulationThread.joinable()) {
		simulationThread.join();
	}

	// for(OrderCard* t = buyHead; t; t = t->next) {

	// 	time_t tt = chrono::system_clock::to_time_t(t->arriveTime);
	// 	cout << "BUY id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << " arrive time=" << put_time(localtime(&tt), "%Y-%m-%d %H:%M:%S") << "\n";
		
	// }
	// for(OrderCard* t = sellHead; t; t = t->next) {

	// 	time_t tt = chrono::system_clock::to_time_t(t->arriveTime);
	// 	cout << "SELL id=" << t->orderID <<" price=" << t->price << " qty=" << t->quantity << " arrive time=" << put_time(localtime(&tt), "%Y-%m-%d %H:%M:%S") << "\n";

	// }

}
