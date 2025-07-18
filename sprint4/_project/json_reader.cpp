#include "json_reader.h"
#include "request_handler.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
/**
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
using namespace json_reader;
using namespace std::string_literals;

void JsonReader::WriteBuses (const Bus& bus){
    Bus bus_temp = bus;
    if (!bus_temp.is_roundtrip){
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

const json::Node& JsonReader::FindInJson(const std::string &s){
    auto p = root_.AsDict().find(s);
    if (p == root_.AsDict().end()){
        throw std::runtime_error ("Cannot find " + s + " in root map!");
    }
    return p->second;
}

void JsonReader::ReadForBaseRequests(void){
    for (auto const& e: FindInJson("base_requests").AsArray()){
        std::string type = e.AsDict().at("type").AsString();
        if (type == "Stop"){
            Stop stop;
            for (auto const& request: e.AsDict()){
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
                    for (auto it = request.second.AsDict().begin(); it != request.second.AsDict().end(); it++) {
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
            for (auto const& request: e.AsDict()){
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
}

void JsonReader::ReadForStatRequests(void) {
    for (auto const& e: FindInJson("stat_requests").AsArray()){
        if (!e.IsDict()) {
            throw std::runtime_error ("Elements array of stat_requests aren't maps!");
        }
        Request request;
        for (auto const& [f, s]: e.AsDict()){
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
                if (s.AsString() == "Map"){
                    request.type = "Map";
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
}

svg::Color JsonReader::GetColor(const json::Node &node){
    //svg::Color color;
    if (node.IsArray()){
        const auto& ar = node.AsArray();
        if (ar.size() == 3){
            svg::Rgb rgb;
            rgb.red = ar.at(0).AsInt();
            rgb.green = ar.at(1).AsInt();
            rgb.blue = ar.at(2).AsInt();
            return svg::Color{rgb};
        }
        if (ar.size() == 4){
            svg::Rgba rgba;
            rgba.red = ar.at(0).AsInt();
            rgba.green = ar.at(1).AsInt();
            rgba.blue = ar.at(2).AsInt();
            rgba.opacity = ar.at(3).AsDouble();
            return svg::Color{rgba};
        }
    }
    if (node.IsString()){
        const auto& str = node.AsString();
        return svg::Color{str};
    }
    throw std::runtime_error ("Unexpected color format");
}

map_renderer::RenderSettings
JsonReader::ReadForMapRenderer(void){
    map_renderer::RenderSettings settings;

    for (auto const& [f, s]: FindInJson("render_settings").AsDict()){
        if (f == "width"){
            settings.width = s.AsDouble();
            continue;
        }
        if (f == "padding"){
            settings.padding = s.AsDouble();
            continue;
        }
        if (f == "height"){
            settings.height = s.AsDouble();
            continue;
        }
        if (f == "line_width"){
            settings.line_width = s.AsDouble();
            continue;
        }
        if (f == "stop_radius"){
            settings.stop_radius = s.AsDouble();
            continue;
        }
        if (f == "bus_label_font_size"){
            settings.bus_label_font_size = s.AsInt();
            continue;
        }
        if (f == "bus_label_offset"){
            settings.bus_label_offset.first = s.AsArray().at(0).AsDouble();
            settings.bus_label_offset.second = s.AsArray().at(1).AsDouble();
            continue;
        }
        if (f == "stop_label_font_size"){
            settings.stop_label_font_size = s.AsInt();
            continue;
        }
        if (f == "stop_label_offset"){
            settings.stop_label_offset.first = s.AsArray().at(0).AsDouble();
            settings.stop_label_offset.second = s.AsArray().at(1).AsDouble();
            continue;
        }
        if (f == "underlayer_color"){
            settings.underlayer_color = GetColor(s);
            continue;
        }
        if (f == "color_palette"){
            for (auto const &e: s.AsArray()){
                settings.color_palette.push_back(GetColor(e));
            }
            continue;
        }
        if (f == "underlayer_width"){
            settings.underlayer_width = s.AsDouble();
            continue;
        }
        throw std::runtime_error ("Unexpected: " + f + " render_settings!");
    }
    return settings;
}

transport_catalogue::TransportCatalogue JsonReader::CreateTransportCatalogue(){
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::TransportCatalogue::distanceBtwStops_t distancesBtwStops;
    ReadForBaseRequests();
    for (auto& cmd : stops_) {
        domain::Stop stop {cmd.name, cmd.coord};

        catalogue.AddStop(std::move(stop));
        for (auto [f, s]: cmd.road_distances)
            distancesBtwStops.insert({{cmd.name, f}, s});
        distancesBtwStops.insert({{cmd.name, cmd.name}, 0});
    }

    for (auto& cmd : buses_) {
        catalogue.AddBus(cmd.name, cmd.stops_sv, cmd.is_roundtrip);
    }

    for (auto const& [pair, m]: distancesBtwStops){
        catalogue.AddDistanceBtwStops (pair, m);
    }

    return catalogue;
}

std::vector<std::variant<StopResponse, BusResponse, MapResponse>> 
JsonReader::CalculateRequests (const transport_catalogue::TransportCatalogue& cat){
    std::vector<std::variant<StopResponse, BusResponse, MapResponse>> responses;
    ReadForStatRequests();
    for (auto const& e: requests_){
        if (e.type == "Stop"){
            responses.emplace_back(StopResponse{e.id, cat.GetBusesForStop(e.name)});
            continue;
        }
        if (e.type == "Bus"){
            responses.emplace_back(BusResponse{e.id, cat.GetRouteStatistics(e.name)});
            continue;
        }
        if (e.type == "Map"){
            request_handler::RequestHandler rhandler{cat};
            map_renderer::MapRenderer mapRenderer{ReadForMapRenderer(), rhandler};
            responses.emplace_back(MapResponse{e.id, mapRenderer.RenderMap()});
            continue;
        }
        throw std::runtime_error ("Unknown request!");
    }
    return responses;
}

json::Node SerializeResponse(const MapResponse& value) {
    return json::Builder{}
            .StartDict()
                .Key("map"s).Value(value.svg)
                .Key("request_id"s).Value(value.id)
            .EndDict()
            .Build();
}

json::Node SerializeResponse(const StopResponse& value) {
    json::Builder builder;
                builder.StartDict()
                    .Key("request_id"s).Value(value.id);
    if (value.pbuses == nullptr){
        builder.Key("error_message"s).Value("not found"s);
    } else {
        builder.Key("buses"s).StartArray();
        for (auto const& e: *(value.pbuses)){
            builder.Value(std::string{e});
        }
        builder.EndArray();
    }
    return builder.EndDict().Build();
}

json::Node SerializeResponse(const BusResponse& value) {
    json::Builder builder;
                builder.StartDict()
                    .Key("request_id"s).Value(value.id);
    if (!(value.stat.has_value())) {
       builder.Key("error_message"s).Value("not found"s);
    } else {
        auto const& stat_value = value.stat.value();
        builder.Key("curvature"s).Value(stat_value.curvature);
        builder.Key("route_length"s).Value(static_cast<int>(stat_value.trajectory));
        builder.Key("stop_count"s).Value(stat_value.totalStops);
        builder.Key("unique_stop_count"s).Value(stat_value.uniqueStops);
    }
    return builder.EndDict().Build();
}

json::Document
json_reader::TransformRequestsIntoJson(const std::vector<std::variant<StopResponse, BusResponse, MapResponse>>& requests){
    json::Array array;
    for (auto const& e: requests){
        std::visit([&array](const auto& value) {
            array.push_back(SerializeResponse(value));
        }, e);
    }
    return json::Document{json::Node{std::move(array)}};
}
