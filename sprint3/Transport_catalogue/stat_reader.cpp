#include "stat_reader.h"
#include "input_reader.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <iomanip>
CommandDescription ParseRequest(std::string_view request){
    size_t start_pos = request.find_first_not_of(" "); //" Bus  14  "
    request.remove_prefix(start_pos); //"Bus  14  "
    start_pos = request.find_first_of(" ");
    auto command = request.substr(0, start_pos);
    request.remove_prefix(start_pos);//"  14  "
    start_pos = request.find_first_not_of(" ");
    request.remove_prefix(start_pos);//"14  "
    //size_t end_pos = request.find_first_of(" ");

    //return request;
    return {std::string(command),
            std::string(request),
            ""};
}

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output) {
    const auto v = ParseRequest(request);
    if (v.command == "Bus") {
        std::vector<std::deque<Stop>::const_iterator> Stops_for_bus;
        try {
            Stops_for_bus = tansport_catalogue.GetStopsForBus(v.id);
        } catch (...){
            //Bus 751: not found
            output << "Bus " << v.id << ": not found" << std::endl;
            return;
        }
        //Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
        //print
        double distance {0};
        std::unordered_set<std::string_view> unique_stops;
        for (const auto& stop : Stops_for_bus){
            unique_stops.insert(stop->name);
        }
        for (size_t i = 0; i < Stops_for_bus.size() - 1; ++i){
            distance += ComputeDistance(Stops_for_bus[i]->coordinates, Stops_for_bus[i + 1]->coordinates);
        }

        output << "Bus " << v.id << ": " << Stops_for_bus.size() << " stops on route, ";
        output << unique_stops.size() << " unique stops, ";
        output << std::setprecision(6) << distance << " route length" << std::endl;
    }
}