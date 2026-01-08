#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <algorithm>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>  // Required for std::tie
#include <variant>
#include <vector>

#include "app.h"
#include "http_server.h"
#include "responses.h"

namespace api_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
namespace fs = std::filesystem;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;

// 1. Data Transfer Objects (DTOs)
struct JoinGameReq {
    std::string user_name;
    std::string map_id;
};
struct JoinGameResult {
    std::string token;
    int player_id;
};
// 2. Logic Result (Success or specific Error)
enum class JoinError { None, InvalidName, MapNotFound, JsonParseError };

// Helper remains inline because it's simple and used by the template operator()
std::vector<std::string_view> SplitTarget(std::string_view target);

using JoinOutcome = std::variant<JoinGameResult, JoinError>;

class HandleAPI {
public:
    HandleAPI(model::Game game) : game_{game} {}

    template <typename Request>
    response::ResponseVariant operator()(const Request& req) {
        const auto& target = req.target();

        if (target == "/api/v1/game/join") {
            return HandleJoin(req);
        }
        if (target == "/api/v1/game/players") {
            return HandlePlayers(req);
        }

        if (target == "/api/v1/maps") {
            return response::MakeJSON(http::status::ok, std::move(HandleMaps()), req);
        }

        if (target.starts_with("/api/v1/maps/")) {
            auto parts = SplitTarget(target);
            if (parts.size() == 4) {
                auto [response_body, error] = HandleMapId(parts[3]);
                if (error) {
                    return response::MakeError(http::status::not_found, "mapNotFound", "map Not Found", req);
                }
                return response::MakeJSON(http::status::ok, std::move(response_body), req);
            }
        }

        return response::MakeError(http::status::bad_request, "badRequest", "Invalid API path", req);
    }

private:
    template <class Body, class Allocator>
    response::ResponseVariant HandleJoin(const http::request<Body, http::basic_fields<Allocator>>& req) {
        if constexpr (std::is_same_v<Body, http::string_body>) {
            if (req.method() != http::verb::post) {
                return response::MakeMethodNotAllowedError("Only POST method is expected"s, "POST", req);
            }

            if (req.body().empty())
                return response::MakeError(
                    http::status::bad_request, "invalidArgument"s, "Join game request parse error"s, req);

            auto AuthReq = ParseJSONAuthReq(req.body());
            if (!AuthReq)
                return response::MakeError(
                    http::status::bad_request, "invalidArgument"s, "Join game request parse error"s, req);
            JoinOutcome outcome = ProcessJoinGame(*AuthReq);

            if (std::holds_alternative<JoinError>(outcome)) {
                auto err = std::get<JoinError>(outcome);
                if (err == JoinError::MapNotFound)
                    return response::MakeError(http::status::not_found, "mapNotFound", "Map not found", req);
                else
                    return response::MakeError(http::status::bad_request, "invalidArgument", "Invalid name", req);
            }

            const auto& success = std::get<JoinGameResult>(outcome);
            boost::json::object json_body;
            json_body["authToken"] = success.token;
            json_body["playerId"] = success.player_id;
            return response::MakeJSON(http::status::ok, std::move(json_body), req);
        } else if constexpr (std::is_same_v<Body, http::empty_body>) {
            return response::MakeError(http::status::bad_request, "invalidArgument", "Invalid name", req);
        } else if constexpr (std::is_same_v<Body, http::file_body>) {
            static_assert(true);
        } else {
            static_assert(std::is_same_v<Body, void>, "Unsupported Body type");
        }
        return response::MakeError(http::status::bad_request, "bad_request"s, "bad_request"s, req);
    }

    response::ResponseVariant HandlePlayers(const http::request<http::string_body>& req) {
        if (req.method() != http::verb::get && req.method() != http::verb::head) {
            return response::MakeMethodNotAllowedError("Invalid method"s, "GET, HEAD"s, req);
        }
        auto it = req.find(http::field::authorization);
        if (it == req.end()) {
            return response::MakeError(
                http::status::unauthorized, "invalidToken"s, "Authorization header is missing"s, req);
        }
        auto auth = it->value();
        constexpr std::string_view prefix = "Bearer ";
        if (!auth.starts_with(prefix)) {
            return response::MakeError(
                http::status::unauthorized, "invalidToken"s, "Authorization header is missing"s, req);
        }
        std::string token{auth.substr(prefix.size())};
        if (token.empty()) {
            return response::MakeError(
                http::status::unauthorized, "invalidToken"s, "Authorization header is missing"s, req);
        }
        auto json_players = GetPlayers(token);
        if (!json_players) {
            return response::MakeError(http::status::unauthorized, "unknownToken"s, "Token is missing"s, req);
        }

        return response::MakeJSON(http::status::ok, std::move(*json_players), req);
    }
    /*
        template <class Body, class Allocator>
        response::ResponseVariant HandlePlayers(const http::request<Body, http::basic_fields<Allocator>>& req) {
            if constexpr (std::is_same_v<Body, http::empty_body>) {
                if (req.method() != http::verb::get && req.method() != http::verb::head) {
                    return response::MakeMethodNotAllowedError("Invalid method"s, "GET, HEAD"s, req);
                }
                auto it = req.find(http::field::authorization);
                if (it == req.end()) {
                    return response::MakeError(
                        http::status::unauthorized, "invalidToken"s, "Authorization header is missing"s, req);
                }

                auto auth = it->value();
                constexpr std::string_view prefix = "Bearer ";
                if (!auth.starts_with(prefix)) {
                    return response::MakeError(
                        http::status::unauthorized, "invalidToken"s, "Authorization header is missing"s, req);
                }
                std::string token = auth.substr(prefix.size());
                if (!token.empty()) {
                    return response::MakeError(
                        http::status::unauthorized, "invalidToken"s, "Authorization header is missing"s, req);
                }

                boost::json::object json_body;
                return response::MakeJSON(http::status::ok, std::move(json_body), req);
            } else if constexpr (std::is_same_v<Body, http::string_body>) {
                    return response::MakeMethodNotAllowedError("Invalid method"s, "GET, HEAD"s, req);
            } else {
                return response::MakeError(http::status::bad_request, "bad_request"s, "bad_request"s, req);
                // static_assert(std::is_same_v<Body, void>, "Unsupported Body type");
            }
            return response::MakeError(http::status::bad_request, "bad_request"s, "bad_request"s, req);
        }
    */
    model::Game game_;
    json::value HandleMaps();
    std::optional<app::AuthRequest> ParseJSONAuthReq(std::string body);
    JoinOutcome ProcessJoinGame(const app::AuthRequest& params);
    std::optional<boost::json::value> GetPlayers(std::string token);
    std::pair<json::value, bool> HandleMapId(std::string_view name_map);
    boost::json::object SerializeMap(const model::Map& map);
    boost::json::object SerializeRoad(const model::Road& road);
    boost::json::object SerializeBuilding(const model::Building& b);
    boost::json::object SerializeOffice(const model::Office& o);
};

}  // namespace api_handler
