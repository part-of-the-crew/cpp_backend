#pragma once


#include <string>
#include <vector>
#include <deque>
#include <unordered_map>

#include "geo.h"

struct Stop {
	std::string name;
	Coordinates coordinates;
};


class TransportCatalogue {

	std::deque<Stop> stops;
	std::unordered_map<std::string, std::deque<Stop>::const_iterator> stops;

    std::vector <std::deque<Stop>::const_iterator> route;
	
public:
    // Методы для добавления и получения маршрутов и остановок
    void AddStop(const Stop& stop) { 
        stops.push_back(stop); 
    };
    void AddRoute(std::string name, std::string id, std::string decr);

};
