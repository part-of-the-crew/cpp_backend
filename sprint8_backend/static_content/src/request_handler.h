#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <tuple>  // Required for std::tie
// #include <boost/url.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>

#include "http_server.h"
#include "model.h"

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;

// Helper remains inline because it's simple and used by the template operator()
inline std::vector<std::string_view> SplitTarget(std::string_view target) {
    std::vector<std::string_view> result;
    while (!target.empty()) {
        if (target.front() == '/') {
            target.remove_prefix(1);
            continue;
        }
        auto pos = target.find('/');
        result.push_back(target.substr(0, pos));
        if (pos == std::string_view::npos)
            break;
        target.remove_prefix(pos + 1);
    }
    return result;
}

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    constexpr static std::string_view APP_JSON = "application/json"sv;
    constexpr static std::string_view TEXT_PLAIN = "text/plain"sv;

    constexpr static std::string_view IMAGE_PNG = "image/png"sv;
    constexpr static std::string_view APP_OCT_STREAM = "application/octet-stream"sv;
};



// Template functions MUST stay in the header
template <class Request>
http::response<http::string_body> MakeTextResponse(http::status status, std::string body, const Request& req,
                                                   std::string_view content_type) {
    http::response<http::string_body> res{status, req.version()};
    res.set(http::field::content_type, content_type);
    res.body() = std::move(body);
    res.prepare_payload();
    res.keep_alive(req.keep_alive());
    return res;
}

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game, const std::filesystem::path& path_to_static) 
                : game_{game}
                , path_to_static_ {path_to_static}
                {}
    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    // This is a template, so it stays in the header
    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        http::status status{};
        std::string body{};
        std::string content_type{};
        std::string_view target{req.target()};

        switch (req.method()) {
            case http::verb::get:

                if (target.starts_with("/api")) {
                    ;
                    status = HandleAPI(target, body, content_type);
                } else {
                    status = HandleStatic(target, body, content_type);
                }

            case http::verb::head:
                break;

                /*
                        case http::verb::post:
                            // Handle POST
                            break;
                        case http::verb::put:
                            // Handle PUT
                            break;
                        case http::verb::delete_:
                            // Handle DELETE
                            break;
                        case http::verb::options:
                            // Handle OPTIONS
                            break;
                        case http::verb::patch:
                            // Handle PATCH
                            break;
                */
            default:
                // Method not supported
                status = http::status::method_not_allowed;
                // allow = "GET, HEAD"sv;
                body = "Invalid method"s;
                break;
        }

        send(MakeTextResponse(status, body, req, content_type));
    }

private:
    model::Game& game_;
    const std::filesystem::path& path_to_static_;

    http::status HandleAPI(const std::string_view target, std::string& body, std::string& content_type);
    http::status HandleStatic(const std::string_view target, std::string& body, std::string& content_type);

    std::string HandleMaps();
    std::pair<std::string, bool> HandleMapId(std::string_view name_map);
    std::string HandleErrorRequest(std::string_view code, std::string_view message);

    boost::json::object SerializeMap(const model::Map& map);
    boost::json::object SerializeRoad(const model::Road& road);
    boost::json::object SerializeBuilding(const model::Building& b);
    boost::json::object SerializeOffice(const model::Office& o);
};

}  // namespace http_handler
