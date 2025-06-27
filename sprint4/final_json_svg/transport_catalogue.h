#pragma once


#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <optional>

#include "geo.h"

namespace transport_catalogue {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    bool operator==(const Stop& other) const {
        return (other.name == name) && (other.coordinates == coordinates);
    }
    bool operator<(const Stop& rhs) {
        return name < rhs.name;
    }
    bool operator<(const Stop& rhs) const {
        return name < rhs.name;
    }
};

struct Bus {
    std::string name;
    std::vector<std::deque<Stop>::const_iterator> stops;
    bool is_roundtrip;
    bool operator<(const Bus& rhs) {
        return name < rhs.name;
    }
    bool operator<(const Bus& rhs) const {
        return name < rhs.name;
    }
};

struct RouteStatistics {
    std::string_view busName;
    double trajectory;
    double curvature;
    int uniqueStops;
    int totalStops;
};

class TransportCatalogue {
    struct PtrPtrHasher {
        size_t operator() (const std::pair<const Stop*, const Stop*> &p) const {
            std::size_t h1 = std::hash<const Stop*>()(p.first);
            std::size_t h2 = std::hash<const Stop*>()(p.second);
            return (h1 << 1) ^ (h2);
        }
    };
    struct PtrStrHasher {
        size_t operator() (const std::pair<const Stop*, std::string> &p) const {
            std::size_t h1 = std::hash<const Stop*>()(p.first);
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
    std::deque<Stop> stops;
    std::unordered_map<std::string_view, std::deque<Stop>::const_iterator> stopname_to_stop;
    std::unordered_map<std::string_view, std::set<std::string_view>> stopname_to_bus;

    std::deque <Bus> routes;
    std::unordered_map<std::string_view, std::deque<Bus>::const_iterator> busname_to_route;

    std::unordered_map<std::pair<const Stop*, const Stop*>, int, PtrPtrHasher> distances;

public:

    using distanceBtwStops_t = std::unordered_map<std::pair<std::string, std::string>, int, StrStrHasher>;
    void AddStop(const Stop& stop);

    void AddBus(const std::string& name, const std::vector<std::string_view>& stops_list, bool is_roundtrip);
    const Bus* GetStopsForBus(std::string_view busname) const;

    
    std::vector<const Bus*> GetStopsForAllBuses(void) const;
    std::set<std::string_view> GetAllStopNames(void) const;
    std::set<std::string_view> GetAllBusNames(void) const;

    const std::set<std::string_view>* GetBusesForStop(std::string_view stopName) const;
    std::optional<RouteStatistics> GetRouteStatistics(std::string_view busName) const;
    void AddDistanceBtwStops (const std::pair<std::string, std::string>& p, int m);

};

}//transport_catalogue