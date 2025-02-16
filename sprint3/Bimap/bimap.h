#pragma once

#include <optional>
#include <string_view>
#include <memory>

class BiMap {
public:
    bool Add(std::string_view key, std::string_view value);
    std::optional<std::string_view> FindValue(std::string_view key) const noexcept;

    std::optional<std::string_view> FindKey(std::string_view value) const noexcept;
    BiMap(BiMap&&) noexcept;
    BiMap& operator=(BiMap&&) noexcept;
    ~BiMap() noexcept;
    BiMap() noexcept;
    BiMap(const BiMap&);
    BiMap& operator=(const BiMap&);
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};