#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"
#include "input_reader.h"

namespace statistics {
void ParseAndPrint(const transport_catalogue::TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output);

void ReadAndPrintRequests(const transport_catalogue::TransportCatalogue &transport_catalogue, std::istream &in, 
                        std::ostream &out);

CommandDescription ParseRequest(std::string_view request);

} // namespace statistics