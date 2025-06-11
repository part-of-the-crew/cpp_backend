#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>

#include "transport_catalogue.h"
#include "json.h"
#include "geo.h"

namespace json_reader {

struct Request {
public:
    std::string name;
    virtual ~Request() = default;

    bool operator<(const Request& rhs) {
        return name < rhs.name;
    }
};

struct Bus : public Request {
public:
    std::vector <std::string> stops;
    bool is_roundtrip = false;

};

struct Stop : public Request {
public:
    geo::Coordinates coord;
    std::map <std::string, int> road_distances;
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
private:
    const json::Document& doc_;
    void WriteBuses (const Bus& bus);
    void WriteStops (const Stop& bus);
    std::set <Bus> buses_;
    std::set <Stop> stops_;
};

void JsonReader::WriteBuses (const Bus& bus){
    buses_.emplace(bus);
}

void JsonReader::WriteStops (const Stop& stop){
    stops_.emplace(stop);
}

void JsonReader::ReadFromJson(void){
    auto& const root = doc_.GetRoot();
    if (root.IsMap()){
        for (auto& const p: root.AsMap()){
            if (!p.second.IsArray()){
                throw std::runtime_error ("Elements of root aren't arays!");
            }
            //ParseJson (catalogue, p);
            if (p.first == "base_requests"){
                for (auto& const e: p.second.AsArray()){
                    std::string type = e.AsMap().at("type").AsString();
                    if (type == "Stop"){
                        Stop stop;
                        for (auto& const request: e.AsMap()){
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
                                    stop.road_distances.emplace(std::move(it->first), it->second);
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
                        for (auto& const request: e.AsMap()){
                            if (request.first == "name"){
                                bus.name = request.second.AsString();
                                continue;
                            }
                            if (request.first == "stops"){
                                for (auto stop: request.second.AsArray()) {
                                    bus.stops.emplace_back(std::move(stop));
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
                for (auto& const e: p.second.AsArray()){
                    
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
        for (auto [f, s]: cmd.road_distances))
            distancesBtwStops.insert({{cmd.id, f}, s});
        distancesBtwStops.insert({{cmd.id, cmd.id}, 0});
    }

    for (auto& cmd : buses_) {
        catalogue.AddBus(cmd.id, parsing::ParseRoute(cmd.description));
    }

    for (auto const& [pair, m]: distancesBtwStops){
        catalogue.AddDistanceBtwStops (pair, m);
    }
    return catalogue;
}

} //namespace json_reader