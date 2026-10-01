#pragma once

#include <optional>
#include <string>
#include <sstream>
#include <stdexcept>

#include "marketevent.h"

class CSVParser {
public:
    std::optional<MarketEvent> parse_line(const std::string& line) const;
};