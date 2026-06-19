#include "pnltracker.h"
#include "tradelog.h"
#include <iostream>
#include <algorithm>
using namespace std;

PnLTracker tracker;

void PnLTracker::processFill() {
    for(auto it : tradeLog) {

        if (it.side == 'B') {
            if (net_position >= 0) {
                // Opening or adding to a long
                avg_entry_price = (avg_entry_price * net_position + it.price * it.quantity) 
                                / (net_position + it.quantity);
            } 
            else {
                // Closing a short position
                if (it.quantity > -net_position) {
                    // Fix: The buy order is bigger than our short position.
                    // int remaining_qty = it.quantity - (-net_position);
                    realized_pnl += (avg_entry_price - it.price) * (-net_position);
                    
                    // Reset the average price
                    avg_entry_price = it.price; 
                } else {
                    // Normal short cover (doesn't cross over to positive)
                    realized_pnl += (avg_entry_price - it.price) * it.quantity;
                }
            }
            net_position += it.quantity;
        } 
        
        else if (it.side == 'S') {
            if (net_position <= 0) {
                // Opening or adding to a short
                avg_entry_price = (avg_entry_price * (-net_position) + it.price * it.quantity) 
                                / ((-net_position) + it.quantity);
            } 
            else {
                // Closing a long position
                if (it.quantity > net_position) {
                    // Fix: The sell order is bigger than our long position.
                    // int remaining_qty = it.quantity - net_position;  
                    realized_pnl += (it.price - avg_entry_price) * net_position;
                    
                    // Reset the average price
                    avg_entry_price = it.price;
                } else {
                    // Normal long close (doesn't cross over to negative)
                    realized_pnl += (it.price - avg_entry_price) * it.quantity;
                }
            }
            net_position -= it.quantity;
        }
    }
}

void PnLTracker::markToMarket(double last_price) {
    if (net_position > 0)
        unrealized_pnl = (last_price - avg_entry_price) * net_position;
    else
        unrealized_pnl = (avg_entry_price - last_price) * (-net_position);
}