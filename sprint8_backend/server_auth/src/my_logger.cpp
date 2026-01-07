#include "my_logger.h"

namespace server_logging {

template <typename Handler>
LoggingRequestHandler::LoggingRequestHandler(Handler&& handler) : handler_(std::forward<Handler>(handler)) {}

void LoggingRequestHandler::operator()(tcp::endpoint& endpoint, http::request<http::string_body>& req,
                                       http_handler::Send&& send) {
    // LOGGING BEFORE
    handler_(endpoint, req, std::forward<http_handler::Send>(send));

    // LOGGING AFTER (optional)
}

}  // namespace server_logging
