#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"
#include "input_reader.h"

namespace statistics {
void ParseAndPrint(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output);

CommandDescription ParseRequest(std::string_view request);

} // namespace statistics