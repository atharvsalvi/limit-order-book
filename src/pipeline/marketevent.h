#pragma once

#include <chrono>

enum class EventType {
    ADD,
    CANCEL
};

enum class Side {
    BUY,
    SELL
};

struct MarketEvent {
    EventType type;

    long id = 0;
    Side side = Side::BUY;

    long quantity = 0;
    double price = 0.0;

    std::chrono::system_clock::time_point timestamp;
};