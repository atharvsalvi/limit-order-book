#pragma once
#include <vector>
#include <string>
using namespace std;

struct Candle {
    string date;
    double open, high, low, close;
    int volume;
};

vector<Candle> loadCSV(const string& path);