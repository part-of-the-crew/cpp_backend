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

svg::Document RequestHandler::RenderMap(){
    CreateProjection();
    AllStopNames = cat_.GetAllStopNames();
    AllBusNames  = cat_.GetAllBusNames();

    svg::Document output_doc;
    for (auto const &e: CreateRoutes()){
        output_doc.Add(e);
    }
    for (auto const &e: CreateBusNames()){
        output_doc.Add(e);
    }
    for (auto const &e: CreateStops()){
        output_doc.Add(e);
    }
    for (auto const &e: CreateStopsNames()){
        output_doc.Add(e);
    }
    return output_doc;
}

void RequestHandler::CreateProjection(void){
    std::vector<geo::Coordinates> v_for_projector;

    for (auto const& e: cat_.GetStopsForAllBuses()){
        for (auto const& stop: e->stops){
            v_for_projector.push_back(stop->coordinates);
        }
    }
    auto proj = std::make_unique<map_renderer::SphereProjector>(
        v_for_projector.begin(), v_for_projector.end(), 
        settings_.width, settings_.height, settings_.padding);

    proj_ = std::move(proj);
}


std::vector<svg::Polyline> RequestHandler::CreateRoutes(void){
    std::vector<svg::Polyline> poly_vec;
    std::size_t icolor = 0;
    for (auto const& el: AllBusNames){
        auto bus = cat_.GetStopsForBus(el);
        svg::Polyline poly;
        for (auto const& stop: bus->stops){
            poly.AddPoint((*proj_)({stop->coordinates.lat, stop->coordinates.lng}))
                .SetStrokeColor(settings_.color_palette[icolor])
                .SetStrokeWidth(settings_.line_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetFillColor(svg::NoneColor)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        }
        poly_vec.emplace_back(std::move(poly));
        icolor++;
        if (icolor == settings_.color_palette.size())
            icolor = 0;
    }

    return poly_vec;
}

std::vector<svg::Text> request_handler::RequestHandler::CreateBusNames(){
    std::vector<svg::Text> text_vec;
    std::size_t icolor = 0;
    for (auto const& el: AllBusNames){
        auto bus = cat_.GetStopsForBus(el);
        auto stop = *(bus->stops.front());
        svg::Text text1, text2;
        text1.SetData(bus->name)
            .SetPosition((*proj_)({stop.coordinates.lat, stop.coordinates.lng}))
            .SetOffset({settings_.bus_label_offset.first, settings_.bus_label_offset.second})
            .SetFontSize(settings_.bus_label_font_size)
            .SetStrokeWidth(settings_.line_width)
            .SetFontFamily("Verdana")
            .SetFontWeight("bold")
            .SetStrokeColor(settings_.color_palette[icolor])
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetFillColor(settings_.color_palette[icolor])
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        text2 = text1;
        text_vec.emplace_back(std::move(text1));

        if (!bus->is_roundtrip){
            text2
                .SetPosition((*proj_)({stop.coordinates.lat, stop.coordinates.lng}));
            auto stop = *(bus->stops.back());
            text_vec.emplace_back(std::move(text2));
        }
        
        icolor++;
        if (icolor == settings_.color_palette.size())
            icolor = 0;
    }
    return text_vec;
}

std::vector<svg::Circle> request_handler::RequestHandler::CreateStops(){
    std::vector<svg::Circle> vec;
    for (auto const& el: AllBusNames){
        auto bus = cat_.GetStopsForBus(el);
        svg::Circle c;
        for (auto const& stop: bus->stops){
            c.SetCenter((*proj_)({stop->coordinates.lat, stop->coordinates.lng}))
                .SetRadius(settings_.stop_radius)
                .SetFillColor("white");
        }
        vec.emplace_back(std::move(c));

    }
    return vec;
}

std::vector<svg::Text> request_handler::RequestHandler::CreateStopsNames(){
    std::vector<svg::Text> vec;
    for (auto const& el: AllStopNames){
        const auto bus = cat_.GetBusesForStop(el);
        if ( nullptr == bus)
            continue;
        auto pstop = cat_.GetStop(el);
        svg::Text text;
        text.SetData(pstop->name)
            .SetPosition((*proj_)({pstop->coordinates.lat, pstop->coordinates.lng}))
            .SetOffset({settings_.stop_label_offset.first, settings_.stop_label_offset.second})
            .SetFontSize(settings_.stop_label_font_size)
            .SetStrokeWidth(settings_.underlayer_width)
            .SetFontFamily("Verdana")
            .SetStrokeColor(settings_.underlayer_color)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetFillColor(settings_.underlayer_color)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        vec.emplace_back(std::move(text));

    }
    return vec;
}
