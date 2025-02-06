#include <stdexcept>

#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const Stop& stop) { 
    stops.push_back(std::move(stop));
    stopname_to_stop[stops.back().name] = stops.cend() - 1;
    stopname_to_bus[stops.back().name] = {};
};

void TransportCatalogue::AddRoute(const std::string& bus, const std::vector<std::string_view>& stops_list){
    routes.push_back({ std::move(bus), {}});
    for ( auto const& stop_name: stops_list){
        const auto it = stopname_to_stop.find(stop_name);
        if (it != stopname_to_stop.end()){
            auto it1 = routes.end() - 1;
            it1->stops.push_back(it->second);
        } else {
            throw std::invalid_argument("Unknown stop name");
        }

        auto it2 = stopname_to_bus.find(stop_name);
        it2->second.push_back(routes.cend() - 1);
    }
    busname_to_route[routes.back().name] = routes.cend() - 1;
}

std::vector<std::deque<Stop>::const_iterator> TransportCatalogue::GetStopsForBus(std::string_view busName) const {
    const auto it = busname_to_route.find(busName);
    if (it == busname_to_route.cend()){
        throw std::invalid_argument("Unknown bus name");
    }
    return it->second->stops;
}

std::vector<std::deque<Bus>::const_iterator> TransportCatalogue::GetBusesForStop(std::string_view stopName) const {
    const auto it = stopname_to_bus.find(stopName);
    if (it == stopname_to_bus.cend()){
        throw std::invalid_argument("Unknown stop name");
    }
    return it->second;
}