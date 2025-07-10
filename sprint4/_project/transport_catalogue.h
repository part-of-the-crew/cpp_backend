#pragma once


#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <optional>

#include "domain.h"

namespace transport_catalogue {


class TransportCatalogue {
    struct PtrPtrHasher {
        size_t operator() (const std::pair<const domain::Stop*, const domain::Stop*> &p) const {
            std::size_t h1 = std::hash<const domain::Stop*>()(p.first);
            std::size_t h2 = std::hash<const domain::Stop*>()(p.second);
            return (h1 << 1) ^ (h2);
        }
    };
    struct PtrStrHasher {
        size_t operator() (const std::pair<const domain::Stop*, std::string> &p) const {
            std::size_t h1 = std::hash<const domain::Stop*>()(p.first);
            std::size_t h2 = std::hash<std::string>()(p.second);
            return (h1 << 1) ^ (h2);
        }
    };

    struct StrStrHasher {
        std::size_t operator()(const std::pair<std::string, std::string>& p) const {
            std::hash<std::string> hasher;
            return hasher(p.first) ^ (hasher(p.second) << 1); // simple hash combine
        }
    };
private:
    std::deque<domain::Stop> stops;
    std::unordered_map<std::string_view, std::deque<domain::Stop>::const_iterator> stopname_to_stop;
    std::unordered_map<std::string_view, std::set<std::string_view>> stopname_to_bus;

    std::deque <domain::Bus> routes;
    std::unordered_map<std::string_view, std::deque<domain::Bus>::const_iterator> busname_to_route;

    std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, int, PtrPtrHasher> distances;

public:

    using distanceBtwStops_t = std::unordered_map<std::pair<std::string, std::string>, int, StrStrHasher>;
    void AddStop(const domain::Stop& stop);
    void AddDistanceBtwStops (const std::pair<std::string, std::string>& p, int m);
    void AddBus(const std::string& name, const std::vector<std::string_view>& stops_list, bool is_roundtrip);

    const domain::Bus* GetStopsForBus(std::string_view busname) const;
    const domain::Stop* GetStop(std::string_view stopname) const;
    
    std::vector<const domain::Bus*> GetStopsForAllBuses(void) const;
    std::set<std::string_view> GetAllStopNames(void) const;
    std::set<std::string_view> GetAllBusNames(void) const;

    const std::set<std::string_view>* GetBusesForStop(std::string_view stopName) const;
    std::optional<domain::RouteStatistics> GetRouteStatistics(std::string_view busName) const;


};

}//transport_catalogue