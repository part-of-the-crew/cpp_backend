#pragma once
#include <boost/json/array.hpp>
#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <charconv>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "app.h"
#include "responses.h"

namespace api_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;

// 2. Logic Result (Success or specific Error)
enum class JoinError { None, InvalidName, MapNotFound, JsonParseError };

// Helper remains inline because it's simple and used by the template operator()
std::vector<std::string_view> SplitTarget(std::string_view target);

using JoinOutcome = std::variant<json::object, JoinError>;

struct RecordSetting {
    int start = 0;
    int maxItems = 100;
};
inline std::optional<RecordSetting> ParseRecordSetting(std::string_view str) {
    RecordSetting result;

    auto query_pos = str.find('?');
    if (query_pos == std::string_view::npos) {
        return std::nullopt;
    }

    str = str.substr(query_pos + 1);

    while (!str.empty()) {
        auto ampersand_pos = str.find('&');
        auto pair = str.substr(0, ampersand_pos);

        auto equals_pos = pair.find('=');
        if (equals_pos != std::string_view::npos) {
            auto key = pair.substr(0, equals_pos);
            auto value = pair.substr(equals_pos + 1);
            int parsed_val = 0;
            auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), parsed_val);

            if (ec == std::errc()) {
                if (key == "start") {
                    result.start = parsed_val;
                } else if (key == "maxItems") {
                    result.maxItems = parsed_val;
                } else {
                    throw std::invalid_argument("Invalid key in query string");
                }
            } else {
                throw std::invalid_argument("Invalid value in query string");
            }
        }
        if (ampersand_pos == std::string_view::npos) {
            break;
        }
        str = str.substr(ampersand_pos + 1);
    }

    return result;
}

class HandleAPI {
public:
    explicit HandleAPI(app::Application& app) : app_(app) {}

    response::ResponseVariant operator()(const http::request<http::string_body>& req);

private:
    response::ResponseVariant HandleMaps(const http::request<http::string_body>& req);
    response::ResponseVariant HandleMapId(const http::request<http::string_body>& req);
    response::ResponseVariant HandleJoin(const http::request<http::string_body>& req);
    response::ResponseVariant HandleState(const http::request<http::string_body>& req);
    response::ResponseVariant HandlePlayers(const http::request<http::string_body>& req);
    response::ResponseVariant HandlePlayerAction(const http::request<http::string_body>& req);
    response::ResponseVariant HandleTick(const http::request<http::string_body>& req);
    response::ResponseVariant HandleRecords(const http::request<http::string_body>& req);

    std::optional<std::string> ExtractToken(const http::request<http::string_body>& req);
    app::Application& app_;

    std::optional<app::AuthRequest> ParseJSONAuthReq(std::string body);

    JoinOutcome ProcessJoinGame(const app::AuthRequest& params);
    std::string ProcessPlayers(const std::string& token);
    std::optional<std::string> ProcessState(const app::Token& token);

    json::object SerializeMap(const model::Map& map);
    json::object SerializeRoad(const model::Road& road);
    json::object SerializeBuilding(const model::Building& b);
    json::object SerializeOffice(const model::Office& o);
    json::array SerializeLoots(const std::string& loot);
    json::object SerializeLootInMap(const app::Player& player) const;
    json::array SerializePlayerBag(const model::Dog* dog) const;
    json::array SerializeRecords(const std::vector<domain::RetiredPlayer>& records) const;
};

}  // namespace api_handler