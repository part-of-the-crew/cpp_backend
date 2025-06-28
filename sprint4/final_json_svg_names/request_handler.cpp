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
        if (nullptr == bus)
            continue;
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
    std::vector<svg::Text> vec;
    std::size_t icolor = 0;
    for (auto const& el: AllBusNames){
        auto bus = cat_.GetStopsForBus(el);
        if (nullptr == bus)
            continue;
        auto stop1 = *(bus->stops.front());
        svg::Text text1, text2;
        svg::Text undertext1, undertext2;
        auto point = (*proj_)({stop1.coordinates.lat, stop1.coordinates.lng});
        text1.SetData(bus->name)
            .SetPosition(point)
            .SetOffset({settings_.bus_label_offset.first, settings_.bus_label_offset.second})
            .SetFontSize(settings_.bus_label_font_size)
            .SetFontFamily("Verdana")
            .SetFontWeight("bold")
            .SetFillColor(settings_.color_palette[icolor]);
        undertext1 = text1;
        undertext1
            .SetFillColor(settings_.underlayer_color)
            .SetStrokeColor(settings_.underlayer_color)  
            .SetStrokeWidth(settings_.underlayer_width)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        text2 = text1;
        undertext2 = undertext1;
        vec.emplace_back(std::move(undertext1));
        vec.emplace_back(std::move(text1));

        if (!bus->is_roundtrip){
            auto it = bus->stops.rbegin() + bus->stops.size()/2;
            if (it != bus->stops.rend()){
                auto stop2 = *(it);
                if (stop1.name != stop2->name){
                    text2
                        .SetPosition((*proj_)({stop2->coordinates.lat, stop2->coordinates.lng}));
                    undertext2
                        .SetPosition((*proj_)({stop2->coordinates.lat, stop2->coordinates.lng}));
                    vec.emplace_back(std::move(undertext2));
                    vec.emplace_back(std::move(text2));
                }
            }
        }
        
        icolor++;
        if (icolor == settings_.color_palette.size())
            icolor = 0;
    }
    return vec;
}

std::vector<svg::Circle> request_handler::RequestHandler::CreateStops(){
    std::vector<svg::Circle> vec;
    for (auto const& el: AllStopNames){
        const auto bus = cat_.GetBusesForStop(el);
        if (nullptr == bus || bus->empty())
            continue;
        const auto stop = cat_.GetStop(el);
        svg::Circle c;
        c.SetCenter((*proj_)({stop->coordinates.lat, stop->coordinates.lng}))
            .SetRadius(settings_.stop_radius)
            .SetFillColor("white");
        vec.emplace_back(std::move(c));
    }
    return vec;
}

std::vector<svg::Text> request_handler::RequestHandler::CreateStopsNames(){
    std::vector<svg::Text> vec;
    for (auto const& el: AllStopNames){
        const auto bus = cat_.GetBusesForStop(el);
        if ( nullptr == bus || bus->empty())
            continue;
        auto pstop = cat_.GetStop(el);
        svg::Text text;
        svg::Text undertext;
        text.SetData(pstop->name)
            .SetPosition((*proj_)({pstop->coordinates.lat, pstop->coordinates.lng}))
            .SetOffset({settings_.stop_label_offset.first, settings_.stop_label_offset.second})
            .SetFontSize(settings_.stop_label_font_size)
            .SetFontFamily("Verdana")
            .SetFillColor("black");
        undertext = text;
        undertext
            .SetFillColor(settings_.underlayer_color)
            .SetStrokeColor(settings_.underlayer_color)  
            .SetStrokeWidth(settings_.underlayer_width)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        vec.emplace_back(std::move(undertext));
        vec.emplace_back(std::move(text));

    }
    return vec;
}
