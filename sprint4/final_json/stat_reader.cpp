#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <iomanip>

#include "stat_reader.h"

namespace statistics {
CommandDescription ParseRequest(std::string_view request){
    size_t start_pos = request.find_first_not_of(' ');
    request.remove_prefix(start_pos);
    start_pos = request.find_first_of(' ');
    auto command = request.substr(0, start_pos);
    request.remove_prefix(start_pos);
    start_pos = request.find_first_not_of(' ');
    request.remove_prefix(start_pos);

    return {std::string(command),
            std::string(request),
            ""};
}

void ParseAndPrint(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    const auto v = ParseRequest(request);
    if (v.command == "Bus") {
        //Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
        const auto stat = transport_catalogue.GetRouteStatistics(v.id);
        if (!stat.has_value()) {
            //Bus 751: not found
            output << "Bus " << v.id << ": not found" << std::endl;
            return;
        }
        auto const& stat_value = stat.value();
        output << "Bus " << stat_value.busName << ": " << stat_value.totalStops << " stops on route, ";
        output << stat_value.uniqueStops << " unique stops, ";
        output << std::setprecision(6) << stat_value.trajectory << " route length, " << 
        stat_value.curvature << " curvature" << std::endl;
        return;
    }
    if (v.command == "Stop") {
        /*
        Stop Samara: not found
        Stop Prazhskaya: no buses
        Stop Biryulyovo Zapadnoye: buses 256 828
        */
        output << "Stop " << v.id << ": ";
        const auto& stops_for_bus = transport_catalogue.GetBusesForStop(v.id);
        if (stops_for_bus == nullptr){
            output << "not found" << std::endl;
            return;
        }
        if (stops_for_bus->empty()) {
                output << "no buses" << std::endl;
                return;
        }
        output << "buses ";
        for (const auto& bus : *stops_for_bus) {
            output << bus << " ";
        }
        output << std::endl;
        return;
    }
}

void ReadAndPrintRequests(const TransportCatalogue &transport_catalogue, std::istream &in, 
                                std::ostream &out){
    int base_request_count;
    in >> base_request_count >> std::ws;

    for (int i = 0; i < base_request_count; ++i) {
        std::string line;
        std::getline(in, line);
        ParseAndPrint(transport_catalogue, line, out);
    }
}

} //namespace