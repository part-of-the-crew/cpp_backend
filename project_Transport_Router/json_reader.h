#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <sstream>

#include "transport_catalogue.h"
#include "json_builder.h"
#include "geo.h"
#include "map_renderer.h"
#include "transport_router.h"

namespace json_reader {

struct Enquire {
public:
    std::string name;
    virtual ~Enquire() = default;

    bool operator<(const Enquire& rhs) {
        return name < rhs.name;
    }
    bool operator<(const Enquire& rhs) const {
        return name < rhs.name;
    }
};

struct Bus : public Enquire {
public:
    std::vector <std::string> stops;
    std::vector <std::string_view> stops_sv;
    bool is_roundtrip = false;

};

struct Stop : public Enquire {
public:
    geo::Coordinates coord;
    std::map <std::string, int> road_distances;
};

struct Request : public Enquire {
public:
    int id;
    std::string type;
    std::string from;
    std::string to;
};

struct Response {
    int id;
};

struct StopResponse : public Response {
    const std::set<std::string_view>* pbuses;
};

struct BusResponse : public Response{
    std::optional<domain::RouteStatistics> stat;
};
struct MapResponse : public Response {
    const std::string svg;
};

struct RouteResponse  : public Response {
    std::optional<domain::Route> route;
};


/*
struct RequestPtrComparator {
    bool operator()(const std::shared_ptr<Request>& lhs, const std::shared_ptr<Request>& rhs) const {
        return lhs->name < rhs->name;
    }
};
*/
class JsonReader {
public:
    JsonReader (const json::Document& doc): root_{doc.GetRoot()}{
        if (!root_.IsDict())
            throw std::runtime_error ("Root is not a Map!");
    };

    transport_catalogue::TransportCatalogue CreateTransportCatalogue(void);
    std::vector<std::variant<StopResponse, BusResponse, MapResponse, RouteResponse>> 
    CalculateRequests (const transport_catalogue::TransportCatalogue& cat);

private:
    //json::Document doc_;
    const json::Node& root_;
    std::set <Bus> buses_;
    std::set <Stop> stops_;
    std::vector <Request> requests_;
    void ReadBaseRequests(void);
    void ReadStatRequests(void);
    svg::Color GetColor(const json::Node &node);
    const json::Node& FindInJson(const std::string &s);
    void WriteBuses (const Bus& bus);
    void WriteStops (const Stop& bus);

    std::optional<map_renderer::RenderSettings> ReadMapRenderer(void);
    std::optional<transport_router::RoutingSettings> ReadRoutingSettings(void);
};

json::Document
TransformRequestsIntoJson(const std::vector<std::variant<StopResponse, BusResponse, MapResponse, RouteResponse>>& requests);


} //namespace json_reader