#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include <algorithm>

#include "transport_catalogue.h"

using namespace transport_catalogue;
using namespace std::string_literals;

void TransportCatalogue::AddStop(const Stop& stop) { 
    stops.push_back(std::move(stop));
    stopname_to_stop[stops.back().name] = stops.cend() - 1;
    stopname_to_bus[stops.back().name] = {};
}
void TransportCatalogue::AddDistanceBtwStops (const std::pair<std::string, std::string>& p, int m) {
    const Stop* ptr1 = &(*stopname_to_stop[p.first]);
    const Stop* ptr2 = &(*stopname_to_stop[p.second]);
    distances.insert({{ptr1, ptr2 }, m});
}

void TransportCatalogue::AddBus(const std::string& bus, 
                                const std::vector<std::string_view>& stops_list,
                                bool is_roundtrip){
    routes.push_back({ std::move(bus), {}, is_roundtrip});
    for ( auto const& stop_name: stops_list){
        const auto it = stopname_to_stop.find(stop_name);
        if (it != stopname_to_stop.end()){
            auto it1 = routes.end() - 1;
            it1->stops.push_back(it->second);
        } else {
            throw std::invalid_argument("Unknown stop name"s + std::string(stop_name) + std::to_string(stops_list.size()));
        }
/*
        std::cout << bus << " " << std::endl;
        for (auto e: stops_list)
        {
            std::cout << e << " " << std::endl;
        }
        return;
*/
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
        throw std::invalid_argument("Division by 0 "s + std::to_string(p->stops.size()) + 
            std::to_string(unique_stops.size()) + " " + std::to_string(stops.size())
        + " " + std::to_string(routes.size()));
    }
    for (size_t i = 0; i < v.size() - 1; ++i){
        auto it = distances.find({&(*v[i]), &(*v[i + 1])});
        if (it == distances.end()) {
            it = distances.find({&(*v[i + 1]), &(*v[i])});
            if (it == distances.end()){
                throw std::invalid_argument ("Help with "s + v[i]->name + "<->"s + v[i + 1]->name);
            }
        }
        trajectory += it->second;
    }
    return RouteStatistics{busName, trajectory, trajectory/distance, 
            static_cast<int>(unique_stops.size()), static_cast<int>(v.size())};
}

std::vector<const Bus *>
transport_catalogue::TransportCatalogue::GetStopsForAllBuses(void) const
{
    std::vector<const Bus *> vBuses;
    for (auto [f, s] : busname_to_route){
        vBuses.push_back(&(*s));
    }
    std::sort(vBuses.begin(), vBuses.end(), [](const auto& a, const auto& b)
                                            {
                                            return *a < *b;
                                            });  
    return vBuses;
}

std::set<std::string_view> 
transport_catalogue::TransportCatalogue::GetAllStopNames(void) const
{
    std::set<std::string_view> index;
    for (auto [f, s]: stopname_to_stop){
        index.insert(f);
    }
    return index;
}
std::set<std::string_view> 
transport_catalogue::TransportCatalogue::GetAllBusNames(void) const
{
    std::set<std::string_view> index;
    for (auto [f, s]: busname_to_route){
        index.insert(f);
    }
    return index;
}
const Stop *transport_catalogue::TransportCatalogue::GetStop(std::string_view stopname) const
{
    auto it = stopname_to_stop.find(stopname);
    if (it == stopname_to_stop.end())
        return nullptr;

    return &(*it->second);
}

