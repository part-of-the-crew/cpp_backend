#include "extra_data.h"

#include <stdexcept>

namespace extra_data {

const std::vector<LootType>& ExtraData::GetMapValue(const std::string& name) const {
    auto it = extra_.find(name);
    if (it == extra_.end()) {
        throw std::out_of_range("GetMapValue: map not found: " + name);
    }
    return it->second;
}

std::optional<unsigned long> ExtraData::GetNumberLootforMap(const std::string& name) const {
    auto it = extra_.find(name);
    if (it == extra_.end())
        return std::nullopt;
    const auto& vec = it->second;
    return static_cast<unsigned long>(vec.size());
}

void ExtraData::AddMapLoot(std::string name, std::vector<LootType> v) {
    // move-assign or insert
    extra_.insert_or_assign(std::move(name), std::move(v));
}

bool ExtraData::Contains(const std::string& name) const {
    return extra_.find(name) != extra_.end();
}

std::size_t ExtraData::Size() const noexcept {
    return extra_.size();
}

}  // namespace extra_data
