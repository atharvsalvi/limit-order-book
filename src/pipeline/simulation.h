#pragma once

#include "marketevent.h"
#include "engine/orderbook.h"

class Simulation {
public:
    void process(const MarketEvent& event);
};

void run_simulation(const std::string& path);