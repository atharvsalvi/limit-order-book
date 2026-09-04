// publisher.h
#pragma once
#include <vector>
#include <mutex>

struct Level {
    double price;
    int quantity;
};

struct OrderbookSnapshot {
    std::vector<Level> bids;
    std::vector<Level> asks;
    double last_trade_price = 0.0;
};

// Use 'extern' so all files know these exist, but we don't define them here
extern std::mutex slideMutex;
extern OrderbookSnapshot latestSnapshot;
extern bool hasNewData;

// Declare our two main functions
void updateSnapshotSlide();
void publisherWorker();