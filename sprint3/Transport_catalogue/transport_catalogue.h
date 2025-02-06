#pragma once


#include <string>
#include <vector>
#include <deque>
#include <unordered_map>

#include "geo.h"

struct Stop {
	std::string name;
	Coordinates coordinates;
    bool operator==(const Stop& other) const {
        return (other.name == name) && (other.coordinates == coordinates);
    }
};

struct Bus {
	std::string name;
    std::vector<std::deque<Stop>::const_iterator> stops;
};

class TransportCatalogue {
public:
	std::deque<Stop> stops;
	std::unordered_map<std::string_view, std::deque<Stop>::const_iterator> stopname_to_stop;

    std::deque <Bus> routes;
	std::unordered_map<std::string_view, std::deque<Bus>::const_iterator> busname_to_route;
	
public:
    // Методы для добавления и получения маршрутов и остановок
    void AddStop(const Stop& stop);

    void AddRoute(const std::string& name, const std::vector<std::string_view>& stops_list);
    std::vector<std::deque<Stop>::const_iterator> GetStopsForBus(std::string_view busname) const;
};
