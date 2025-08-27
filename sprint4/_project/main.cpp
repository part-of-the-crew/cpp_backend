#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include "log_duration.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

void some_tests(void){

    ;
}

int main() {

    {
        LOG_DURATION("overall duration");
        json::Document input_doc = json::Load(std::cin);
        json_reader::JsonReader jreader{input_doc};
        //
        //json::Print(json::Load(std::cin), std::cerr);
        //return 0;

        transport_catalogue::TransportCatalogue cat = jreader.CreateTransportCatalogue();

        auto preparedRequests = jreader.CalculateRequests(cat);


        json::Document output_doc = json_reader::TransformRequestsIntoJson(preparedRequests);
        json::Print(output_doc, std::cout);
    }


    //json::Print(output_doc, std::cerr);
}
