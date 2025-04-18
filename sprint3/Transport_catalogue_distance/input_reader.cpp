#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream>
#include <string>

namespace parsing {
/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

std::unordered_map<std::string, int> ParseDistances(std::string_view sv) {
    std::unordered_map<std::string, int> result;
    
    sv.remove_prefix(sv.find_first_of(',') + 1);
    size_t pos = sv.find_first_of(',');
    if (pos == std::string::npos)
        return result;
    sv.remove_prefix(pos);    
    sv.remove_prefix(sv.find_first_of(',') + 2);
    while (sv.size() > 1) {
        int m = std::stoi(std::string(sv));
        sv.remove_prefix(sv.find_first_of('m') + 5);
        pos = std::min(sv.find_first_of(','), sv.size());
        auto stop = sv.substr(0, pos);
        result.insert({std::string(stop), m});
        sv.remove_prefix(pos);
        sv.remove_prefix(std::min(sv.find_first_of(' '), sv.size()));
    }
    return result;
}
/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

CommandDescription ParseCommandLine(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

} // namespace

void InputReader::ReadInput(std::istream &in)
{
    int base_request_count;
    in >> base_request_count >> std::ws;

    for (int i = 0; i < base_request_count; ++i) {
        std::string line;
        std::getline(in, line);
        ParseLine(line);
    }
}

void InputReader::ParseLine(std::string_view line)
{
    auto command_description = parsing::ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) {

    InputReader::ReorderCommands();
    TransportCatalogue::distanceBtwStops_t distancesBtwStops;
    for (auto& cmd : commands_) {
        if (cmd.command == "Stop") {
            Stop stop {cmd.id, parsing::ParseCoordinates(cmd.description)};          
            catalogue.AddStop(std::move(stop));

            for (auto [f, s]: parsing::ParseDistances(cmd.description))
                distancesBtwStops.insert({{cmd.id, f}, s});

            distancesBtwStops.insert({{cmd.id, cmd.id}, 0});
            continue;
        }
        if (cmd.command == "Bus") {
            catalogue.AddBus(cmd.id, parsing::ParseRoute(cmd.description));
            continue;
        }
    }

    for (auto const& [pair, m]: distancesBtwStops){
        catalogue.AddDistanceBtwStops (pair, m);
    }
}
void InputReader::PrintCommands() const
{
    for (const auto& command_ : commands_) {
        std::cout << command_.command << "_" << command_.id << "_" << command_.description << "\n";
    }
    std::cout << "\n";
}

void InputReader::ReorderCommands( void ) {
    std::sort(commands_.rbegin(), commands_.rend());
}
