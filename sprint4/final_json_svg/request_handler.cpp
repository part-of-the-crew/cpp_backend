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

std::vector<svg::Polyline> 
request_handler::CreatePolygons(const transport_catalogue::TransportCatalogue &cat,
                                const map_renderer::RenderSettings& settings)
{
    std::vector<std::vector<geo::Coordinates>> vec;
    std::vector<geo::Coordinates> v_for_projector;
    std::vector<svg::Polyline> poly_vec;
    for (auto const& e: cat.GetStopsForAllBuses()){
        std::vector<geo::Coordinates> v;
        for (auto const& stop: e->stops){
            v.push_back(stop->coordinates);
            v_for_projector.push_back(stop->coordinates);
        }
        vec.emplace_back(v);
    }

    const map_renderer::SphereProjector proj{
        v_for_projector.begin(), v_for_projector.end(), settings.width, settings.height, settings.padding
    };
/*
    for (const auto &geo_coord: v_for_projector) {
        const svg::Point screen_coord = proj(geo_coord);
        std::cout << '(' << geo_coord.lat << ", "sv << geo_coord.lng << ") -> "sv;
        std::cout << '(' << screen_coord.x << ", "sv << screen_coord.y << ')' << std::endl;
    }
*/
    //std::cerr << settings.padding << std::endl;
    std::size_t icolor = 0;
    for (auto const& el: vec){
        svg::Polyline poly;
        for (auto const& coordinates: el){

            poly.AddPoint(proj(coordinates))
                .SetStrokeColor(settings.color_palette[icolor])
                .SetStrokeWidth(settings.line_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetFillColor(svg::NoneColor)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        }
        poly_vec.emplace_back(std::move(poly));
        icolor++;
        if (icolor == settings.color_palette.size())
            icolor = 0;
    }

    return poly_vec;
}
