#include "parser.h"

std::optional<MarketEvent>
CSVParser::parse_line(const std::string& line) const {

    if (line.empty()) {
        return std::nullopt;
    }

    std::string clean_line = line;

    // Remove surrounding quotes if they exist.
    if (clean_line.size() >= 2 &&
        clean_line.front() == '"' &&
        clean_line.back() == '"') {

        clean_line = clean_line.substr(
            1,
            clean_line.size() - 2
        );
    }

    std::stringstream ss(clean_line);

    std::string command;

    if (!std::getline(ss, command, ',')) {
        return std::nullopt;
    }

    try {

        // -------------------------
        // ADD
        // -------------------------
        if (command == "ADD") {

            std::string id_str;
            std::string side_str;
            std::string quantity_str;
            std::string price_str;

            if (!std::getline(ss, id_str, ',') ||
                !std::getline(ss, side_str, ',') ||
                !std::getline(ss, quantity_str, ',') ||
                !std::getline(ss, price_str, ',')) {

                return std::nullopt;
            }

            MarketEvent event;

            event.type = EventType::ADD;
            event.id = std::stol(id_str);
            event.quantity = std::stol(quantity_str);
            event.price = std::stod(price_str);

            if (side_str == "BUY") {
                event.side = Side::BUY;
            }
            else if (side_str == "SELL") {
                event.side = Side::SELL;
            }
            else {
                return std::nullopt;
            }

            event.timestamp =
                std::chrono::system_clock::now();

            return event;
        }

        // -------------------------
        // CANCEL
        // -------------------------
        if (command == "CANCEL") {

            std::string id_str;

            if (!std::getline(ss, id_str, ',')) {
                return std::nullopt;
            }

            MarketEvent event;

            event.type = EventType::CANCEL;
            event.id = std::stol(id_str);

            event.timestamp =
                std::chrono::system_clock::now();

            return event;
        }

    }
    catch (const std::exception&) {
        return std::nullopt;
    }

    return std::nullopt;
}