#pragma once
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>
#include <optional>

#include "geo.h"
/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
namespace domain {

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

}  // namespace domain