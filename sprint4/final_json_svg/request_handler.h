#pragma once

#include <string>
#include <vector>

#include "svg.h"
#include "transport_catalogue.h"
#include "map_renderer.h"


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

    RequestHandler(const Catalogue& cat, const map_renderer::RenderSettings& settings)
                    : cat_{cat}
                    , settings_{settings}
                    {}
    // Возвращает информацию о маршруте (запрос Bus)
   // std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    //const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap(void);
    void GetTransformedRoutes(void);

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const Catalogue& cat_;
    const map_renderer::RenderSettings& settings_;
    std::vector<std::vector<svg::Point>> routes_;
    std::vector<svg::Polyline> CreatePolygons();
};

} //namespace request_handler