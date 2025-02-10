#pragma once


#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <optional>

#include "geo.h"

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    bool operator==(const Stop& other) const {
        return (other.name == name) && (other.coordinates == coordinates);
    }
};

struct Bus {
    std::string name;
    std::vector<std::deque<Stop>::const_iterator> stops;
};

struct RouteStatistics {
    std::string_view busName;
    double distance;
    int uniqueStops;
    int totalStops;
};

class TransportCatalogue {

    std::deque<Stop> stops;
    std::unordered_map<std::string_view, std::deque<Stop>::const_iterator> stopname_to_stop;
    std::unordered_map<std::string_view, std::set<std::string_view>> stopname_to_bus;

    std::deque <Bus> routes;
    std::unordered_map<std::string_view, std::deque<Bus>::const_iterator> busname_to_route;


public:
    // Методы для добавления и получения маршрутов и остановок
    void AddStop(const Stop& stop);

    void AddRoute(const std::string& name, const std::vector<std::string_view>& stops_list);
    std::optional<std::vector<std::deque<Stop>::const_iterator>> 
    GetStopsForBus(std::string_view busname) const;
    std::optional<std::set<std::string_view>> 
    GetBusesForStop(std::string_view stopName) const;
    std::optional<RouteStatistics> GetRouteStatistics(std::string_view busName) const;
};
