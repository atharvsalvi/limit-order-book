#include <fstream>
#include <sstream>
#include <vector>
#include "parser.h"
using namespace std;

vector<Candle> loadCSV(const string& path) {
    vector<Candle> data;
    ifstream file(path);
    string line;
    getline(file, line); // skip header
    while(getline(file, line)) {
        stringstream ss(line);
        Candle c;
        string symbol, series, prev_close, last, vwap, turnover, trades, deliv, deliv_pct;
        
        getline(ss, c.date,    ',');
        getline(ss, symbol,    ',');
        getline(ss, series,    ',');
        getline(ss, prev_close,',');
        ss >> c.open;  ss.ignore();
        ss >> c.high;  ss.ignore();
        ss >> c.low;   ss.ignore();
        getline(ss, last,      ',');
        ss >> c.close; ss.ignore();
        getline(ss, vwap,      ',');
        ss >> c.volume;
    
        data.push_back(c);
    }
    return data;
}