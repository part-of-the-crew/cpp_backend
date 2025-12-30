#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/date_time.hpp>
#include <boost/json.hpp>
#include <boost/log/core.hpp>         // для logging::core
#include <boost/log/expressions.hpp>  // для выражения, задающего фильтр
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/trivial.hpp>  // для BOOST_LOG_TRIVIAL
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std::literals;

namespace logger {
namespace logging = boost::log;
namespace json = boost::json;
namespace keywords = logging::keywords;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;

/*
При запуске сервера:
message — строка server started;
data — объект с полями:
port — порт, на котором работает сервер (обычно 8080),
address — адрес интерфейса (обычно 0.0.0.0).
example:
{
    "timestamp":"2022-09-17T01:04:47.841269",
    "data":
        {
            "port":8080,
            "address":"0.0.0.0"
        },
    "message":"server started"
}
*/

BOOST_LOG_ATTRIBUTE_KEYWORD(json_data, "JsonData", json::value)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)

void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm) {
    boost::json::value jv = {{"timestamp", to_iso_extended_string(*rec[timestamp])},
                             {"data", rec[json_data]->as_object()},
                             {"message", "server started"}};

    boost::json::object obj;
    auto ts = logging::extract<boost::posix_time::ptime>("TimeStamp", rec);
    auto data = logging::extract<boost::json::value>("JsonData", rec);

    if (!ts || !data)
        return;

    obj["timestamp"] = to_iso_extended_string(*ts);
    obj["data"] = (*data).as_object();
    obj["message"] = "server started";

    strm << json::serialize(obj);
}

inline void InitBoostLogFilter() {
    auto sink = boost::make_shared<sinks::synchronous_sink<sinks::text_ostream_backend>>();

    // Write to std::cout
    sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::cout, boost::null_deleter{}));

    sink->set_formatter(&MyFormatter);

    logging::core::get()->add_sink(sink);
    logging::add_common_attributes();
}

inline void logServerLaunch(const boost::asio::ip::tcp::endpoint& endpoint) {
    json::object data_obj;
    data_obj["port"] = endpoint.port();
    data_obj["address"] = endpoint.address().to_string();

    json::value data = std::move(data_obj);

    // attach JsonData attribute with the object, TimeStamp is added by add_common_attributes
    BOOST_LOG_TRIVIAL(info) << logging::add_value(json_data, data);
}

}  // namespace logger
