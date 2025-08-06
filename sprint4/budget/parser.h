#pragma once

#include <ctime>
#include <chrono>
/*
Earn 2000-01-02 2000-01-06 20
ComputeIncome 2000-01-01 2001-01-01
PayTax 2000-01-02 2000-01-03
*/

#include <string>
#include <string_view>
#include <vector>
#include <istream>

std::string FormatDouble(double value);

// Splits a string into words by whitespace using string_view
std::vector<std::string_view> SplitIntoWordsView(std::string_view str);

// Parses a datetime string into std::tm
std::tm parse_datetime_in_tm(std::string datetime);

// Parses a datetime string into time_t
std::time_t parse_datetime(std::string datetime);

// Reads an integer from a full line of input
int ReadNumberOnLine(std::istream& input);
