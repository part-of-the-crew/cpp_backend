#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>

#include "transport_catalogue.h"
#include "json.h"
#include "geo.h"
#include <sstream>

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
};

struct Response {
    int id;
};

struct StopResponse : public Response {
    const std::set<std::string_view>* pbuses;
};

struct BusResponse : public Response{
    std::optional<transport_catalogue::RouteStatistics> stat;
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
        if (!root_.IsMap())
            throw std::runtime_error ("Root is not an Maps!");
        if (!root_.AsMap().begin()->second.IsArray()){
            throw std::runtime_error ("Elements of root aren't arrays!");
        }
    };

    transport_catalogue::TransportCatalogue CreateTransportCatalogue(void);
    std::vector<std::variant<StopResponse, BusResponse>>
    CalculateRequests (const transport_catalogue::TransportCatalogue& cat);
private:
    const json::Node& root_;
    std::set <Bus> buses_;
    std::set <Stop> stops_;
    std::vector <Request> requests_;

    void ReadForBaseRequests(void);
    void ReadForStatRequests(void);
    void ReadForMapRenderer(void);

    const json::Node& FindInJson(const std::string &s);
    void WriteBuses (const Bus& bus);
    void WriteStops (const Stop& bus);
};

json::Document
TransformRequestsIntoJson(const std::vector<std::variant<StopResponse, BusResponse>>& requests);


} //namespace json_reader