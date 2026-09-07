#pragma once
#include <vector>
#include "engine/orderbook.h"

struct PriceLevel {
    double price;
    int quantity;
};

std::vector<PriceLevel> getBidLevels();
std::vector<PriceLevel> getAskLevels();

double getBestBid();
double getBestAsk();

double getSpread();
double getMidPrice();