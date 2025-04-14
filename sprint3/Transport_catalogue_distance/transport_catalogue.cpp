#include <stdexcept>
#include <iostream>
#include <unordered_set>

#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const Stop& stop) { 
    stops.push_back(std::move(stop));
    stopname_to_stop[stops.back().name] = stops.cend() - 1;
    stopname_to_bus[stops.back().name] = {};
}
void TransportCatalogue::ReallocateDistances (distanceBtwStops_t &distancesBtwStops) {
    //distancesBtwStops
    /*
        for (auto& [str, m]: ump){
        buffer_distances.insert({{&stops.back(), std::move(str)}, m});
    }
    buffer_distances.insert({{&stops.back(), stop.name}, 0});
    */
    for (auto [pair, m]: buffer_distances){
        const Stop* ptr = &(*stopname_to_stop[pair.second]);
        auto p_to_ins = std::make_pair(pair.first, ptr);
        distances.insert({p_to_ins, m});
    }
    buffer_distances.clear();
};

void TransportCatalogue::AddBus(const std::string& bus, const std::vector<std::string_view>& stops_list){
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
        auto it3 = routes.end() - 1;
        it2->second.insert(it3->name);
    }
    busname_to_route[routes.back().name] = routes.cend() - 1;
}

const Bus*
TransportCatalogue::GetStopsForBus(std::string_view busName) const {
    const auto it = busname_to_route.find(busName);
    if (it == busname_to_route.cend()){
        return nullptr;
    }
    return &(*it->second);
}

const std::set<std::string_view>* 
TransportCatalogue::GetBusesForStop(std::string_view stopName) const {
    const auto it = stopname_to_bus.find(stopName);
    if (it == stopname_to_bus.cend()) {
        return nullptr;  // Indicate absence
    }
    return &it->second;  // Return pointer to set
}

std::optional<RouteStatistics> 
TransportCatalogue::GetRouteStatistics(std::string_view busName) const {
    double distance {0};
    double trajectory {0};
    std::unordered_set<std::string_view> unique_stops;

    const auto p = GetStopsForBus(busName);
    if (p == nullptr) {
        return std::nullopt;
    }
    const auto v = p->stops;

    for (const auto& stop : v){
        unique_stops.insert(stop->name);
    }
    for (size_t i = 0; i < v.size() - 1; ++i){
        distance += ComputeDistance(v[i]->coordinates, v[i + 1]->coordinates);
    }
    if (0 == distance){
        throw std::invalid_argument("Division by 0");
    }
    for (size_t i = 0; i < v.size() - 1; ++i){
        auto it = distances.find({&(*v[i]), &(*v[i + 1])});
        if (it == distances.end()) {
            it = distances.find({&(*v[i + 1]), &(*v[i])});
            if (it == distances.end()){
                throw std::invalid_argument ("Help");
            }
        }
        trajectory += it->second;
    }
    return RouteStatistics{busName, trajectory, trajectory/distance, 
            static_cast<int>(unique_stops.size()), static_cast<int>(v.size())};
}
