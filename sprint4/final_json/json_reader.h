#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

#include "transport_catalogue.h"
#include "json.h"
#include "geo.h"

namespace json_reader {

struct Bus {
    std::string name;
    std::vector <std::string> stops;
    bool is_roundtrip = false;
};

struct Stop {
    std::string name;
    geo::Coordinates coord;
    std::map <std::string, int> road_distances;
};


void WriteBuses (Bus bus){

}

void WriteStopes (Stop bus){
    
}

TransportCatalogue TransformFromJson(const json::Document& doc){
    TransportCatalogue catalogue;
    auto& const root = doc.GetRoot();
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
                        WriteStopes (stop);
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
                        WriteBuses (bus);
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
    
    return catalogue;
}

void ParseLine(std::string_view line)
{
    auto command_description = TransformFromJson(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

} //namespace json_reader