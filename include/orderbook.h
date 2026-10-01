#pragma once

#include <string>
#include <mutex>

#include"order.h"

extern std::mutex bookMutex;

extern bool isReplaying;

extern OrderCard* buyHead;
extern OrderCard* sellHead;
extern int t;

void addBuyer(OrderCard* buyer, int id, double price, int quantity, TimePoint now);
void addSeller(OrderCard* seller, int id, double price, int quantity, TimePoint now);
void matching_engine();
void remove(int orderID, std::string type);
void cancel_order(int orderID);
Decision strategy_interface();