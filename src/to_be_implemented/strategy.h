#pragma once
#include <vector>
#include <cmath>
#include <numeric>
#include"order.h"

class MeanReversionStrategy {
    private:
        size_t period = 20;       // Lookback period for the moving average
        double numStdDev = 2.0;   // How many standard deviations out to trigger a trade

    public:
        void update(double closePrice);
        Decision generateSignal(double currentPrice);
};

extern std::vector<double> history;