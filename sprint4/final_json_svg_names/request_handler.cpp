#include "request_handler.h"
#include "string_view"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
using namespace std::string_literals;
using namespace std::literals;
using namespace request_handler;

/*
RequestHandler::GetStopsForAllBuses(void){

}
*/
std::vector<geo::Coordinates>
RequestHandler::GetAllCoordinates(void){

    std::vector<geo::Coordinates> v;
    for (auto const& bus: cat_.GetStopsForAllBuses()){
        for (auto const& stop: bus->stops){
            v.push_back(stop->coordinates);
        }
    }
    return v;
}

void RequestHandler::FetchAllEntities(void){
    AllStopNames = cat_.GetAllStopNames();
    AllBusNames = cat_.GetAllBusNames();
}

std::vector<const transport_catalogue::Bus*>
RequestHandler::GetBuses(void){
    std::vector<const transport_catalogue::Bus*> v;
    for (auto const& el: AllBusNames){
        auto bus = cat_.GetStopsForBus(el);
        if (nullptr == bus)
            continue;
        v.push_back(bus);
    }
    return v;
}
std::vector<const transport_catalogue::Stop*>
RequestHandler::GetStops(void){
    std::vector<const transport_catalogue::Stop*> v;
    for (auto const& el: AllStopNames){
        auto bus = cat_.GetBusesForStop(el);
        if (nullptr == bus || bus->empty())
            continue;
        const auto stop = cat_.GetStop(el);
        v.push_back(stop);
    }
    return v;
}