#pragma once
#include <vector>
#include <string>
using namespace std;

struct Candle {
    string date;
    double open, high, low, close;
    int volume;
};

vector<Candle> run_simulation(const string& path);