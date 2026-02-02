#include "serializing_listener.h"

#include <cassert>

namespace ser_listener {

bool SerializingListener::OnTick(std::chrono::milliseconds delta) {
    using namespace std::chrono;

    time_since_save_ += delta;
    if (time_since_save_ < save_period_) {
        return false;
    }
    time_since_save_ -= save_period_;
    SaveStateInFile();
    return true;
}

void SerializingListener::SaveStateInFile() {
    return;
}

}  // namespace ser_listener
