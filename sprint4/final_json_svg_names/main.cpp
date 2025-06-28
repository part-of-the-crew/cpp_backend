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

    auto requests{jreader.CalculateRequests(cat)};
    json::Document output_doc = json_reader::TransformRequestsIntoJson(requests);



    json::Print(output_doc, std::cout);
}
