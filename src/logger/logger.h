#pragma once
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include "orderbook.h"

using namespace std;

class OrderLogger {
    private:
        std::ofstream logFile;
        std::string logPath;

    public:
        OrderLogger(const std::string& path) : logPath(path) {}
        
        bool exists() {
            return std::filesystem::exists(logPath);
        }

        int replay() {
            std::ifstream in(logPath);
            std::string line;
            int lastID = -1;

            while (std::getline(in, line)) {
                std::stringstream ss(line);
                std::string side, idStr, priceStr, qtyStr;

                std::getline(ss, side, ',');
                std::getline(ss, idStr, ',');
                std::getline(ss, priceStr, ',');
                std::getline(ss, qtyStr, ',');

                int id = std::stoi(idStr);
                double price = std::stod(priceStr);
                int qty = std::stoi(qtyStr);
                lastID = std::max(lastID, id);

                OrderCard* order = new OrderCard();
                if (side == "B")
                    addBuyer(order, id, price, qty);
                else
                    addSeller(order, id, price, qty);

                matching_engine();
            }

            in.close();
            return lastID + 1;
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