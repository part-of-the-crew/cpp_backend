#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include "log_duration.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"


int main() {
        json::Document input_doc = json::Load(std::cin);
        json_reader::JsonReader jreader{input_doc};
        transport_catalogue::TransportCatalogue cat = jreader.CreateTransportCatalogue();
        auto preparedRequests = jreader.CalculateRequests(cat);
        json::Document output_doc = json_reader::TransformRequestsIntoJson(preparedRequests);
        json::Print(output_doc, std::cout);
}
