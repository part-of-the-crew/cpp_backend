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
    JsonReader (const json::Document& doc): doc_{doc}{};
    void ReadFromJson(void);
    transport_catalogue::TransportCatalogue CreateTransportCatalogue(void);
    std::vector<std::variant<StopResponse, BusResponse>>
      CalculateRequests (const transport_catalogue::TransportCatalogue& cat);
private:
    const json::Document& doc_;
    void WriteBuses (const Bus& bus);
    void WriteStops (const Stop& bus);
    std::set <Bus> buses_;
    std::set <Stop> stops_;
    std::vector <Request> requests_;
};

void JsonReader::WriteBuses (const Bus& bus){
    Bus bus_temp = bus;
    if (bus_temp.is_roundtrip){
        bus_temp.stops.push_back(bus.stops.front());
    } else {
        for (auto it = bus.stops.crbegin() + 1; it != bus.stops.crend(); ++it) {
            bus_temp.stops.push_back(*it);
            bus_temp.stops_sv.push_back(*it);
        }
    }
    buses_.emplace(bus_temp);
}

void JsonReader::WriteStops (const Stop& stop){
    stops_.emplace(stop);
}

void JsonReader::ReadFromJson(void){
    auto const& root = doc_.GetRoot();
    if (root.IsMap()){
        for (auto const& p: root.AsMap()){
            if (!p.second.IsArray()){
                throw std::runtime_error ("Elements of root aren't arays!");
            }
            //ParseJson (catalogue, p);
            if (p.first == "base_requests"){
                for (auto const& e: p.second.AsArray()){
                    std::string type = e.AsMap().at("type").AsString();
                    if (type == "Stop"){
                        Stop stop;
                        for (auto const& request: e.AsMap()){
                            if ( request.first == "name"){
                                stop.name = request.second.AsString();
                                continue;
                            }
                            if ( request.first == "latitude"){
                                stop.coord.lat = request.second.AsDouble();
                                continue;
                            }
                            if ( request.first == "longitude"){
                                stop.coord.lng = request.second.AsDouble();
                                continue;
                            }
                            if ( request.first == "road_distances"){
                                for (auto it = request.second.AsMap().begin(); it != request.second.AsMap().end(); it++) {
                                    stop.road_distances.emplace(it->first, it->second.AsInt());
                                }
                                continue;
                            }
                            //throw std::runtime_error ("Elements of Stop unknown!");
                        }
                        WriteStops (std::move(stop));
                        continue;
                    }
                    if (type == "Bus"){
                        Bus bus;
                        for (auto const& request: e.AsMap()){
                            if (request.first == "name"){
                                bus.name = request.second.AsString();
                                continue;
                            }
                            if (request.first == "stops"){
                                for (auto stop: request.second.AsArray()) {
                                    bus.stops.emplace_back(stop.AsString());
                                }
                                continue;
                            }
                            if (request.first == "is_roundtrip"){
                                bus.is_roundtrip = request.second.AsBool();
                                continue;
                            }
                            //throw std::runtime_error ("Elements of Bus unknown!");
                        }
                        WriteBuses (std::move(bus));
                        continue;
                    }
                    throw std::runtime_error ("Wrong type:" + type);
                }
                continue;
            } 
            
            if (p.first == "stat_requests"){
                for (auto const& e: p.second.AsArray()){
                    if (!e.IsMap()) {
                        throw std::runtime_error ("Elements array of stat_requests aren't maps!");
                    }
                    Request request;
                    for (auto const& [f, s]: e.AsMap()){
                        if (f == "id"){
                            request.id = s.AsInt();
                            continue;
                        }
                        if (f == "type"){
                            if (s.AsString() == "Stop"){
                                request.type = "Stop";
                                continue;
                            }
                            if (s.AsString() == "Bus"){
                                request.type = "Bus";
                                continue;
                            }
                        }
                        if (f == "name"){
                            request.name = s.AsString();
                            continue;
                        }
                        throw std::runtime_error ("Unknown type in stat_requests!");
                    }
                    requests_.emplace_back(request);

                }
                continue;
            }
            throw std::runtime_error ("Unknown request!");
        }
    } else {
        throw std::runtime_error ("Root is not an Array!");
    }
}

transport_catalogue::TransportCatalogue JsonReader::CreateTransportCatalogue(){
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::TransportCatalogue::distanceBtwStops_t distancesBtwStops;
    for (auto& cmd : stops_) {
        transport_catalogue::Stop stop {cmd.name, cmd.coord};          
        catalogue.AddStop(std::move(stop));
        for (auto [f, s]: cmd.road_distances)
            distancesBtwStops.insert({{cmd.name, f}, s});
        distancesBtwStops.insert({{cmd.name, cmd.name}, 0});
    }

    for (auto& cmd : buses_) {
        catalogue.AddBus(cmd.name, cmd.stops_sv);
    }

    for (auto const& [pair, m]: distancesBtwStops){
        catalogue.AddDistanceBtwStops (pair, m);
    }
    return catalogue;
}

 std::vector<std::variant<StopResponse, BusResponse>> 
 JsonReader::CalculateRequests (const transport_catalogue::TransportCatalogue& cat){
    std::vector<std::variant<StopResponse, BusResponse>> responses;

    for (auto const& e: requests_){
        if (e.type == "Stop"){
            //StopResponse resp{e.id, cat.GetBusesForStop(e.name)};
            responses.emplace_back(StopResponse{e.id, cat.GetBusesForStop(e.name)});
            continue;
        }
        if (e.type == "Bus"){
            //BusResponse resp;
            //resp.id = e.id;
            responses.emplace_back(BusResponse{e.id, cat.GetRouteStatistics(e.name)});
            continue;
        }
        throw std::runtime_error ("Unknown request!");
    }
    return responses;
}

void ProcessRequests(StopResponse value, std::string s){

}

void ProcessRequests(BusResponse value, std::string s){
    
}

std::string Process(std::variant<StopResponse, BusResponse> request) {
    std::string s;
    std::visit(
        [&s](const auto& value) {
            ProcessRequests(value, s);
        },
        request);
    return s;
}


json::Document
TransformRequestsIntoJson(const std::vector<std::variant<StopResponse, BusResponse>>& requests){
    std::string output;
    for (auto const& e: requests){
        output += Process(e);
    }

    std::istringstream strm(output);

    return json::Load(strm);
}

} //namespace json_reader