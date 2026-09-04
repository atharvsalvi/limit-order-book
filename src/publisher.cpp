// publisher.cpp
#include "publisher.h"
#include "orderbook.h" // We include this just to see buyHead and sellHead
#include <iostream>
#include <thread>
#include <chrono>

// 1. Define the actual shared memory variables here (only once!)
std::mutex slideMutex;
OrderbookSnapshot latestSnapshot;
bool hasNewData = false;

// 2. The Translator: Reads your engine's lists and puts them on the slide
void updateSnapshotSlide() {
    std::vector<Level> currentBids;
    std::vector<Level> currentAsks;
    
    // Aggregate Buy side using your global buyHead
    OrderCard* temp = buyHead;
    while(temp != nullptr && currentBids.size() < 10) { 
        if(!currentBids.empty() && currentBids.back().price == temp->price) {
            currentBids.back().quantity += temp->quantity; 
        } else {
            currentBids.push_back({temp->price, temp->quantity});
        }
        temp = temp->next;
    }

    // Aggregate Sell side using your global sellHead
    temp = sellHead;
    while(temp != nullptr && currentAsks.size() < 10) {
        if(!currentAsks.empty() && currentAsks.back().price == temp->price) {
            currentAsks.back().quantity += temp->quantity;
        } else {
            currentAsks.push_back({temp->price, temp->quantity});
        }
        temp = temp->next;
    }

    // Lock the slide and drop the data off
    {
        std::lock_guard<std::mutex> lock(slideMutex);
        latestSnapshot.bids = currentBids;
        latestSnapshot.asks = currentAsks;
        hasNewData = true;
    }
}

// 3. The Delivery Driver: Runs in the background and sends to Supabase
void publisherWorker() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        OrderbookSnapshot snapshotToPublish;
        {
            std::lock_guard<std::mutex> lock(slideMutex);
            if (!hasNewData) continue;
            snapshotToPublish = latestSnapshot; 
            hasNewData = false;
        }

        // TODO: Put your libcurl publish logic right here!
        std::cout << "[Publisher] Sent snapshot to React!" << std::endl;
    }
}