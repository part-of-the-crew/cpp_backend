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
    request_handler::RequestHandler rhandler{cat, render_settings};

    svg::Document output_doc = rhandler.RenderMap();
    output_doc.Render(std::cout);

}
