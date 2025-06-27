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

svg::Document RequestHandler::RenderMap()
{
    GetTransformedRoutes();
    AllStopNames = cat_.GetAllStopNames();
    AllBusNames  = cat_.GetAllBusNames();

    svg::Document output_doc;
    for (auto const &e: CreateRoutes()){
        output_doc.Add(e);
    }
    return output_doc;
}

void RequestHandler::GetTransformedRoutes(void)
{
    std::vector<std::vector<geo::Coordinates>> vec;
    std::vector<geo::Coordinates> v_for_projector;

    for (auto const& e: cat_.GetStopsForAllBuses()){
        std::vector<geo::Coordinates> v;
        for (auto const& stop: e->stops){
            v.push_back(stop->coordinates);
            v_for_projector.push_back(stop->coordinates);
        }
        vec.emplace_back(v);
    }
    auto proj = std::make_unique<map_renderer::SphereProjector>(
        v_for_projector.begin(), v_for_projector.end(), 
        settings_.width, settings_.height, settings_.padding);

    proj_ = std::move(proj);

    for (auto const& e: vec){
        std::vector<svg::Point> v;
        for (auto const& stop: e){
            v.push_back((*proj_)(stop));
        }
        routes_.emplace_back(v);
    }
}

std::vector<svg::Polyline> RequestHandler::CreateRoutes(void)
{
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
/*
std::vector<svg::Polyline> RequestHandler::CreateRoutes(void)
{
    std::vector<svg::Polyline> poly_vec;
    std::size_t icolor = 0;
    for (auto const& el: routes_){
        svg::Polyline poly;
        for (auto const& coordinates: el){
            poly.AddPoint(coordinates)
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
    */
/*
std::vector<svg::Text> request_handler::RequestHandler::CreateBusNames()
{
    std::vector<svg::Text> text_vec;
    for (auto const& el: routes_){
        svg::Text text;
        for (auto const& coordinates: el){
            text.AddPoint(coordinates)
                .SetStrokeColor(settings_.color_palette[icolor])
                .SetStrokeWidth(settings_.line_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetFillColor(svg::NoneColor)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        }
        poly_vec.emplace_back(std::move(text));
    }

    return poly_vec;
}
*/