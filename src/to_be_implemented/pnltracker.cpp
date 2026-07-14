#include "pnltracker.h"
#include "tradelog.h"
#include <iostream>
#include <algorithm>
using namespace std;

size_t last_processed = 0;
PnLTracker tracker;

void PnLTracker::processFill() {
    for(size_t i = last_processed; i < tradeLog.size(); i++) {
        auto& it = tradeLog[i];
        int qty = it.quantity;
        double price = it.price;

        if (it.side == 'B') {
            if (net_position >= 0) {
                // Opening or adding to a long position
                avg_entry_price = (avg_entry_price * net_position + price * qty) 
                                / (net_position + qty);
                net_position += qty;
            } 
            else {
                // Closing a short position (net_position is negative)
                int short_qty = -net_position;
                
                if (qty >= short_qty) {
                    // This fill completely flattens or reverses the short position
                    realized_pnl += (avg_entry_price - price) * short_qty;
                    int remaining_qty = qty - short_qty;
                    
                    if (remaining_qty > 0) {
                        // Position reversed! The remainder starts a brand new Long position
                        net_position = remaining_qty;
                        avg_entry_price = price;
                    } else {
                        // Position is now exactly flat (0)
                        net_position = 0;
                        avg_entry_price = 0.0;
                    }
                } else {
                    // Normal partial short cover (stays short)
                    realized_pnl += (avg_entry_price - price) * qty;
                    net_position += qty;
                }
            }
        } 
        else if (it.side == 'S') {
            if (net_position <= 0) {
                // Opening or adding to a short position
                int current_short_qty = -net_position;
                avg_entry_price = (avg_entry_price * current_short_qty + price * qty) 
                                / (current_short_qty + qty);
                net_position -= qty; // Moves deeper into negative territory
            } 
            else {
                // Closing a long position (net_position is positive)
                int long_qty = net_position;
                
                if (qty >= long_qty) {
                    // This fill completely flattens or reverses the long position
                    realized_pnl += (price - avg_entry_price) * long_qty;
                    int remaining_qty = qty - long_qty;
                    
                    if (remaining_qty > 0) {
                        // Position reversed! The remainder starts a brand new Short position
                        net_position = -remaining_qty;
                        avg_entry_price = price;
                    } else {
                        // Position is now exactly flat (0)
                        net_position = 0;
                        avg_entry_price = 0.0;
                    }
                } else {
                    // Normal partial long close (stays long)
                    realized_pnl += (price - avg_entry_price) * qty;
                    net_position -= qty;
                }
            }
        }
    }
    last_processed = tradeLog.size();
}

void PnLTracker::markToMarket(double last_price) {
    if (net_position > 0)
        unrealized_pnl = (last_price - avg_entry_price) * net_position;
    else
        unrealized_pnl = (avg_entry_price - last_price) * (-net_position);
}