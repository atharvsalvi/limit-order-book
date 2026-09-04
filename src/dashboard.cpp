#include "dashboard.h"

double getBestBid() {
    std::lock_guard<std::mutex> lock(bookMutex);
    return buyHead ? buyHead->price : 0.0;
}

double getBestAsk() {
    std::lock_guard<std::mutex> lock(bookMutex);
    return sellHead ? sellHead->price : 0.0;
}

double getSpread(){
    auto bid = getBestBid();
    auto ask = getBestAsk();

    if (!bid || !ask)
        return 0;

    return ask - bid;
}

double getMidPrice() {
    auto bid = getBestBid();
    auto ask = getBestAsk();

    if (!bid || !ask)
        return 0;

    return (bid + ask) / 2.0;
}

std::vector<PriceLevel> getBidLevels()
{
    std::lock_guard<std::mutex> lock(bookMutex);
    std::vector<PriceLevel> levels;
    OrderCard* current = buyHead;

    while(current)
    {
        if(levels.empty() ||
           levels.back().price != current->price)
        {
            levels.push_back({
                current->price,
                current->quantity
            });
        }
        else
        {
            levels.back().quantity += current->quantity;
        }

        current = current->next;
    }
    return levels;
}

std::vector<PriceLevel> getAskLevels()
{
    std::lock_guard<std::mutex> lock(bookMutex);
    std::vector<PriceLevel> levels;
    OrderCard* current = sellHead;

    while(current)
    {
        if(levels.empty() ||
           levels.back().price != current->price)
        {
            levels.push_back({
                current->price,
                current->quantity
            });
        }
        else
        {
            levels.back().quantity += current->quantity;
        }

        current = current->next;
    }
    return levels;
}