#include <vector>
#include <cmath>
#include <numeric>
#include"order.h"
#include"strategy.h"

std::vector<double> history;

// Call this at the end of every candle loop to store data
void MeanReversionStrategy::update(double closePrice) {
    history.push_back(closePrice);
    if (history.size() > period) {
        history.erase(history.begin()); // Keep window size fixed
    }
}

Decision MeanReversionStrategy::generateSignal(double currentPrice) {
    // We need enough historical data to calculate a valid variance
    if (history.size() < period) {
        return HOLD;
    }

    // 1. Calculate Simple Moving Average (SMA)
    double sum = std::accumulate(history.begin(), history.end(), 0.0);
    double sma = sum / period;

    // 2. Calculate Standard Deviation
    double varianceSum = 0.0;
    for (double p : history) {
        varianceSum += (p - sma) * (p - sma);
    }
    double stdDev = std::sqrt(varianceSum / period);

    // 3. Calculate Bands
    double upperBand = sma + (numStdDev * stdDev);
    double lowerBand = sma - (numStdDev * stdDev);

    // 4. Generate Mean Reversion Signals
    // If price is overextended upwards, expect it to fall back to the mean -> SELL
    if (currentPrice >= upperBand) {
        return SELL;
    }
    // If price is crashed downwards, expect it to bounce back to the mean -> BUY
    else if (currentPrice <= lowerBand) {
        return BUY;
    }

    return HOLD;
}