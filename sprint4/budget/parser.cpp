// Напишите в этом файле код, ответственный за чтение запросов.v
#include "parser.h"
using namespace std::literals;

#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <string_view>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

std::string FormatDouble(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(10) << value;  // high precision to capture decimals
    std::string result = out.str();

    // Trim trailing zeros
    result.erase(result.find_last_not_of('0') + 1);

    // If the string ends with '.', remove it
    if (!result.empty() && result.back() == '.') {
        result.pop_back();
    }

    return result;
}

std::vector<std::string_view> SplitIntoWordsView(std::string_view str) {
    std::vector<std::string_view> result;
    str.remove_prefix(std::min(str.find_first_not_of(" "), str.size()));
    while (!str.empty()) {
        int64_t space = str.find(' ');
        result.push_back(str.substr(0, space));
        str.remove_prefix(std::min(str.find_first_not_of(" ", space), str.size()));
    }
    return result;
}

std::tm parse_datetime_in_tm(std::string datetime) {
    std::tm tm = {};
    std::istringstream ss(datetime);

    // Handle with or without time component
    if (datetime.find(' ') != std::string::npos) {
        ss >> std::get_time(&tm, "%Y-%m-%d %H");
    } else {
        ss >> std::get_time(&tm, "%Y-%m-%d");
    }

    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date/time: " + datetime);
    }

    return tm;
}

std::time_t parse_datetime(std::string datetime) {
    std::tm tm = {};
    std::istringstream ss(datetime);

    // Handle with or without time component
    if (datetime.find(' ') != std::string::npos) {
        ss >> std::get_time(&tm, "%Y-%m-%d %H");
    } else {
        ss >> std::get_time(&tm, "%Y-%m-%d");
    }

    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date/time: " + datetime);
    }

    std::time_t time = std::mktime(&tm);
    if (time == -1) {
        throw std::runtime_error("Error converting date to time_t");
    }
    return time;
}

int ReadNumberOnLine(std::istream& input) {
    std::string line;
    std::getline(input, line);
    return std::stoi(line);
}
