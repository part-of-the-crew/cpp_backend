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

RequestHandler::GetStopsForAllBuses(void){

}

std::vector<geo::Coordinates>
RequestHandler::GetAllCoordinates(void){
        for (auto const& e: requestHandler_.GetStopsForAllBuses()){
        for (auto const& stop: e->stops){
            v_for_projector.push_back(stop->coordinates);
        }
    }
    return v;
}

RequestHandler::GetBuses(void){

}