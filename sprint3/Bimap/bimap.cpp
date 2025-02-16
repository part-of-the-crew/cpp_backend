#include <unordered_map>
#include "bimap.h"


struct BiMap::Impl {
    bool Add(std::string_view key, std::string_view value);
    std::optional<std::string_view> FindKey(std::string_view key) const noexcept;
    std::optional<std::string_view> FindValue(std::string_view value) const noexcept;
private:
    
    std::unordered_map<std::string, std::string> keys;
    std::unordered_map<std::string, std::string> values;
};



BiMap::BiMap() noexcept
    : impl_(std::make_unique<Impl>()) {
}

BiMap::BiMap(const BiMap &other)
    : impl_(other.impl_ ? std::make_unique<Impl>(*other.impl_) : nullptr)
{
}

BiMap &BiMap::operator=(const BiMap &other)
{
    if (this != std::addressof(other)) {
        impl_ = other.impl_ ? std::make_unique<Impl>(*other.impl_) : nullptr;
    }
    return *this;

}

bool BiMap::Add(std::string_view key, std::string_view value) {
    return impl_->Add(key, value);
}


std::optional<std::string_view> BiMap::FindValue(std::string_view key) const noexcept {
    return impl_->FindValue(key);
}

std::optional<std::string_view> BiMap::FindKey(std::string_view value) const noexcept {
    return impl_->FindKey(value);
}

BiMap::BiMap(BiMap &&) noexcept = default;

BiMap &BiMap::operator=(BiMap &&) noexcept = default;

BiMap::~BiMap() noexcept = default;

bool BiMap::Impl::Add(std::string_view _key, std::string_view _value)
{
    std::string key{_key};
    std::string value{_value};
    if (keys.count(key) || values.count(value)) {
        return false;  // Prevent duplicate key or value
    }
    keys[key] = value;
    values[value] = key;
    return true;
}

std::optional<std::string_view> BiMap::Impl::FindKey(std::string_view strv) const noexcept
{
    std::string str{strv};
    auto it = values.find(str);
    if (it != values.end()){
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::string_view> BiMap::Impl::FindValue(std::string_view strv) const noexcept
{
    std::string str{strv};
    auto it = keys.find(str);
    if (it != keys.end()){
        return it->second;
    }
    return std::nullopt;
}