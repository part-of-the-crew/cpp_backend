#include "extra_data.h"

#include <boost/json.hpp>
#include <stdexcept>

namespace extra_data {

std::optional<json::value> ExtraData::GetMapValue(const std::string& name) const {
    auto it = extra_.find(name);
    if (it == extra_.end())
        return std::nullopt;
    return it->second;  // copy of the json::value — fine for tests
}

void ExtraData::AddMapInfo(std::string name, json::value info) {
    if (!info.is_object()) {
        throw std::invalid_argument("AddMapInfo: info must be a JSON object");
    }

    // Ensure the stored object has "id" == name
    // json::object& obj = info.as_object();
    // obj["id"] = name;

    // Insert or assign
    extra_.insert_or_assign(std::move(name), std::move(info));
}

bool ExtraData::Contains(const std::string& name) const {
    return extra_.contains(name);
}

std::size_t ExtraData::Size() const noexcept {
    return extra_.size();
}

void ExtraData::Clear() noexcept {
    extra_.clear();
}
}  // namespace extra_data
