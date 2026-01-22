#pragma once

#include <boost/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>

namespace extra_data {

namespace json = boost::json;

class ExtraData {
public:
    // default constructs an empty object {}
    ExtraData() = default;

    // Find a map by its "id" and return the serialized JSON for that map object.
    // Returns std::nullopt if not found.
    std::optional<json::value> GetMapValue(const std::string& name) const;

    // Throws std::invalid_argument if 'info' cannot be parsed as a JSON object.
    void AddMapInfo(std::string name, json::value info);

    bool Contains(const std::string& name) const;
    std::size_t Size() const noexcept;
    void Clear() noexcept;

private:
    std::unordered_map<std::string, json::value> extra_;
};

}  // namespace extra_data
