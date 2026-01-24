#pragma once

#include <boost/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>

namespace extra_data {

struct LootType {
    std::string name;
    std::string file;
    std::string type;

    int rotation = 0;  // degrees
    std::string color = "#000000";
    double scale = 1.0;
};

class ExtraData {
public:
    // default constructs an empty object {}
    ExtraData() = default;

    const std::vector<LootType>& GetMapValue(const std::string& name) const;
    std::optional<unsigned long> GetNumberLootforMap(const std::string& name) const;
    void AddMapLoot(std::string name, std::vector<LootType>);

    bool Contains(const std::string& name) const;
    std::size_t Size() const noexcept;

private:
    std::unordered_map<std::string, std::vector<LootType>> extra_;
};

}  // namespace extra_data
