#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <memory>

namespace ser_listener {

class ApplicationListener {
public:
    virtual void OnTick(std::chrono::milliseconds delta) = 0;
};

class SerializingListener : public ApplicationListener {
public:
    SerializingListener(std::filesystem::path pathToStateFile, std::chrono::milliseconds save_period)
        : pathToStateFile_(pathToStateFile), save_period_(save_period) {}

    void OnTick(std::chrono::milliseconds delta) override;

private:
    const std::filesystem::path pathToStateFile_;
    std::chrono::milliseconds save_period_;
    std::chrono::milliseconds time_since_save_;
};

}  // namespace ser_listener
