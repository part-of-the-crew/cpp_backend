#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>


#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

void some_tests(void){

    ;
}

int main() {

    some_tests();
    json::Document input_doc = json::Load(std::cin);
    json_reader::JsonReader jreader{input_doc};
    transport_catalogue::TransportCatalogue cat = jreader.CreateTransportCatalogue();
    map_renderer::RenderSettings render_settings = jreader.ReadForMapRenderer();
    std::vector<svg::Polyline> vpol = request_handler::CreatePolygons(cat, render_settings);
    svg::Document output_doc;
    for (auto const &e: vpol){
        output_doc.Add(e);
    }
    output_doc.Render(std::cout);
}
/*
    const double WIDTH = 600.0;
    const double HEIGHT = 400.0;
    const double PADDING = 50.0;
    
    // Точки, подлежащие проецированию
    vector<geo::Coordinates> geo_coords = {
        {43.587795, 39.716901}, {43.581969, 39.719848}, {43.598701, 39.730623},
        {43.585586, 39.733879}, {43.590317, 39.746833}
    };

    // Создаём проектор сферических координат на карту
    const SphereProjector proj{
        geo_coords.begin(), geo_coords.end(), WIDTH, HEIGHT, PADDING
    };

    // Проецируем и выводим координаты
    for (const auto &geo_coord: geo_coords) {
        const svg::Point screen_coord = proj(geo_coord);
        cout << '(' << geo_coord.lat << ", "sv << geo_coord.lng << ") -> "sv;
        cout << '(' << screen_coord.x << ", "sv << screen_coord.y << ')' << endl;
    }
    */