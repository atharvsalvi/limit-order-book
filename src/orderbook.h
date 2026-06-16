#pragma once
#include"order.h"
#include <string>

extern OrderCard* buyHead;
extern OrderCard* sellHead;
extern int t;

void addBuyer(OrderCard* buyer, int id, double price, int quantity);
void addSeller(OrderCard* seller, int id, double price, int quantity);
void matching_engine();
void remove(int orderID, std::string type);
void cancel_order(int orderID);