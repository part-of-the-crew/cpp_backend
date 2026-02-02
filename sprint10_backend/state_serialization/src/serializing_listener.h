#pragma once

#include <chrono>
#include <filesystem>

namespace app {
class Application;
}

namespace ser_listener {

class ApplicationListener {
public:
    virtual ~ApplicationListener() = default;
    virtual bool OnTick(std::chrono::milliseconds delta) = 0;
};

class SerializingListener : public ApplicationListener {
public:
    SerializingListener(std::filesystem::path pathToStateFile, std::chrono::milliseconds save_period)
        : pathToStateFile_(pathToStateFile), save_period_(save_period) {}

    bool OnTick(std::chrono::milliseconds delta) override;
    void SaveStateInFile();
    void SetApplication(const app::Application* app) { app_ = app; }

private:
    const std::filesystem::path pathToStateFile_;
    const app::Application* app_ = nullptr;
    std::chrono::milliseconds save_period_;
    std::chrono::milliseconds time_since_save_;
};

}  // namespace ser_listener
