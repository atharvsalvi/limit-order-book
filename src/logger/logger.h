#pragma once
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include "orderbook.h"

using namespace std;

struct RecoveredOrder {
    char side;
    int id;
    double price;
    int qty;
};

class OrderLogger {
    private:
        std::ofstream logFile;
        std::string logPath;

    public:
        OrderLogger(const std::string& path) : logPath(path) {}
        
        bool exists() {
            return std::filesystem::exists(logPath);
        }

        vector<RecoveredOrder> replay() {
            std::vector<RecoveredOrder> recovered_orders;
            std::ifstream in(logPath);
            std::string line;

            while (std::getline(in, line)) {
                // FIX: Skip empty lines to prevent std::stoi crashes
                if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
                    continue; 
                }

                std::stringstream ss(line);
                std::string sideStr, idStr, priceStr, qtyStr;

                std::getline(ss, sideStr, ',');
                std::getline(ss, idStr, ',');
                std::getline(ss, priceStr, ',');
                std::getline(ss, qtyStr, ',');

                try {
                    RecoveredOrder order;
                    order.side = sideStr[0]; // Assuming side is 'B' or 'S'
                    order.id = std::stoi(idStr);
                    order.price = std::stod(priceStr);
                    order.qty = std::stoi(qtyStr);
                    
                    recovered_orders.push_back(order);
                } 
                catch (const std::exception& e) {
                    std::cerr << "WAL Parsing Error on line: " << line << "\n";
                    // Decide if you want to continue or halt on a corrupted WAL line
                }
            }

            in.close();
            return recovered_orders;
        }

        void openForAppend() {
            logFile.open(logPath, std::ios::app);
        }

        void logOrder(char side, int id, double price, int qty) {
            logFile << side << "," << id << "," << price << "," << qty << "\n";
            logFile.flush();
        }

        ~OrderLogger() {
            if (logFile.is_open()) logFile.close();
        }
};