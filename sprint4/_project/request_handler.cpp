#include "request_handler.h"
#include "string_view"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в domain, ни в json reader.
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
RequestHandler::GetAllCoordinates(void) const {

    std::vector<geo::Coordinates> v;
    for (auto const& bus: cat_.GetStopsForAllBuses()){
        for (auto const& stop: bus->stops){
            v.push_back(stop->coordinates);
        }
    }
    return v;
}

size_t RequestHandler::GetNumberOfRoutePoints(void) const {
    size_t ret = 0;
    for (auto const& bus: cat_.GetStopsForAllBuses()){
        ret += bus->stops.size();
    }
    return ret;
}

void RequestHandler::FetchAllEntities(void){
    AllStopNames = cat_.GetAllStopNames();
    AllBusNames = cat_.GetAllBusNames();
}

std::set<std::string_view> RequestHandler::GetAllStopNames(void) const {
    return AllStopNames;
}

std::vector<const domain::Bus*>
RequestHandler::GetBuses(void) const {
    std::vector<const domain::Bus*> v;
    for (auto const& el: AllBusNames){
        auto bus = cat_.GetStopsForBus(el);
        if (nullptr == bus)
            continue;
        v.push_back(bus);
    }
    return v;
}
std::vector<const domain::Stop*>
RequestHandler::GetStops(void) const {
    std::vector<const domain::Stop*> v;
    for (auto const& el: AllStopNames){
        auto bus = cat_.GetBusesForStop(el);
        if (nullptr == bus || bus->empty())
            continue;
        const auto stop = cat_.GetStop(el);
        v.push_back(stop);
    }
    return v;
}

ssize_t RequestHandler::GetStopNumber(void) const {
    return AllStopNames.size();
}

int RequestHandler::GetDistance(const domain::Stop* from, const domain::Stop* to) const {
    return cat_.GetDistance(from, to).value();
} 