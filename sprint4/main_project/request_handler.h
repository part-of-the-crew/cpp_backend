#pragma once

#include <string>
#include <vector>
#include <memory>


#include "transport_catalogue.h"


namespace request_handler {

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)


class RequestHandler {
public:
    using Catalogue = transport_catalogue::TransportCatalogue;
/*    RequestHandler(const Catalogue& cat, const map_renderer::MapRenderer& mapRenderer)
                    : cat_{cat}
                    , mapRenderer_{mapRenderer}
                    {}
*/
    RequestHandler(const Catalogue& cat)
                    : cat_{cat}
                    {
                        FetchAllEntities();
                    }
    // Возвращает информацию о маршруте (запрос Bus)
   // std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    //const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
   //std::string RenderMap(void);
    //std::vector<const transport_catalogue::Bus*> GetStopsForAllBuses(void) const;
    std::set<std::string_view> GetAllStopNames(void) const;
    std::set<std::string_view> GetAllBusNames(void) const;
    std::vector<geo::Coordinates> GetAllCoordinates(void);
    std::vector<const domain::Bus*>GetBuses(void);
    std::vector<const domain::Stop*>GetStops(void);
private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const Catalogue& cat_;
   // const map_renderer::MapRenderer& mapRenderer_;
    void FetchAllEntities(void);
    std::set<std::string_view> AllStopNames;
    std::set<std::string_view> AllBusNames;
};

} //namespace request_handler