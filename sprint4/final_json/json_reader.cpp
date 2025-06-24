#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

using namespace json_reader;
using namespace std::string_literals;

 void JsonReader::WriteBuses (const Bus& bus){
    Bus bus_temp = bus;
    if (bus_temp.is_roundtrip){
        bus_temp.stops.push_back(bus.stops.front());
    } else {
        for (auto it = bus.stops.crbegin() + 1; it != bus.stops.crend(); ++it) {
            bus_temp.stops.push_back(*it);
        }
    }
    for (const auto& stop : bus_temp.stops) {
        bus_temp.stops_sv.push_back(stop); // safe: before insert
    }
    buses_.emplace(std::move(bus_temp));
}

void JsonReader::WriteStops (const Stop& stop){
    stops_.emplace(stop);
}

void JsonReader::ReadFromJson(void){
    auto const& root = doc_.GetRoot();
    if (root.IsMap()){
        for (auto const& p: root.AsMap()){
            if (!p.second.IsArray()){
                throw std::runtime_error ("Elements of root aren't arrays!");
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
                            //throw std::runtime_error ("Elements of Bus unknown!"s + request.first);
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
    ReadFromJson();
    for (auto& cmd : stops_) {
        transport_catalogue::Stop stop {cmd.name, cmd.coord};

        catalogue.AddStop(std::move(stop));
        for (auto [f, s]: cmd.road_distances)
            distancesBtwStops.insert({{cmd.name, f}, s});
        distancesBtwStops.insert({{cmd.name, cmd.name}, 0});
    }

    for (auto& cmd : buses_) {
        //for (auto e: cmd.stops_sv)
        //    std::cerr << e << std::endl;
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
/*
[
    {
        "buses": [
            "114"
        ],
        "request_id": 1
    },
    {
        "curvature": 1.23199,
        "request_id": 2,
        "route_length": 1700,
        "stop_count": 3,
        "unique_stop_count": 2
    }
]*/
void ProcessRequests(const StopResponse &value, std::string &s){
    s += "\t{\n";
    s += "\t\t\"request_id\": " + std::to_string(value.id);
    s += ",\n";
    if (value.pbuses == nullptr){
        s += "\t\t\"error_message\": \"not found\"\n";
    } else {
        s += "\t\t\"buses\": [\n";
        bool first = 1;
        for (auto const& e: *(value.pbuses)){
            if (!first) {
                s += ",";
            }
            s += "\t\t\t\"";
            s += e;
            s += "\"";
            first = 0;
        }
        s += "\n\t\t]\n";
    }
    s += "\t}\n";
}

void ProcessRequests(const BusResponse &value, std::string &s){
    s += "\t{\n";
    s += "\t\t\"request_id\": " + std::to_string(value.id);
    s += ",\n";
    if (!(value.stat.has_value())) {
        s += "\t\t\"error_message\": \"not found\"\n";
    } else {
        auto const& stat_value = value.stat.value();
        s += "\t\t\"curvature\": ";
        s += std::to_string(stat_value.curvature);
        s += ",\n";
        s += "\t\t\"route_length\": ";
        s += std::to_string(static_cast<int>(stat_value.trajectory));
        s += ",\n";
        s += "\t\t\"stop_count\": " + std::to_string(stat_value.totalStops);
        s += ",\n";
        s += "\t\t\"unique_stop_count\": " + std::to_string(stat_value.uniqueStops);
        s += "\n";
    }
    s += "\t}\n";
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

std::string make_json (const std::vector<std::variant<StopResponse, BusResponse>>& requests){
    std::string output;
    output += "[\n";
    bool first = 1;
    for (auto const& e: requests){
        if (!first) {
            output += "\t,\n";
        }
        output += Process(e);
        first = 0;
    }
    output += "]\n";
    //std::cerr << output;
    return output;
}

json::Document
json_reader::TransformRequestsIntoJson(const std::vector<std::variant<StopResponse, BusResponse>>& requests){
    std::istringstream strm(make_json(requests));
    //std::istringstream strm1("42");
    return json::Load(strm);
}

