

#include "serializing_listener.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <cassert>
#include <fstream>

#include "serialization.h"

using namespace std::literals;
namespace ser_listener {

using InputArchive = boost::archive::text_iarchive;
using OutputArchive = boost::archive::text_oarchive;

bool SerializingListener::OnTick(std::chrono::milliseconds delta) {
    // using namespace std::chrono;
    if (!app_) {
        return false;
    }
    if (save_period_ == std::chrono::milliseconds(0)) {
        return false;
    }
    if (pathToStateFile_.empty()) {
        return false;
    }
    if (delta == std::chrono::milliseconds(0)) {
        return false;
    }
    time_since_save_ += delta;
    if (time_since_save_ < save_period_) {
        return false;
    }
    // time_since_save_ -= save_period_;

    time_since_save_ = {};
    SaveStateInFile();
    return true;
}

std::filesystem::path AddSuffix(std::filesystem::path path, std::string_view suffix) {
    auto stem = path.stem();
    auto ext = path.extension();

    path.replace_filename(stem.string() + suffix.data() + ext.string());

    return path;
}

bool SerializingListener::SaveStateInFile() {
    if (pathToStateFile_.empty()) {
        return false;
    }
    if (!app_) {
        return false;
    }
    serialization::ApplicationRepr repr(*app_);
    std::ofstream output_archive{AddSuffix(pathToStateFile_, ".tmp"sv)};

    OutputArchive ar{output_archive};
    ar << repr;
    output_archive.close();
    std::filesystem::rename(AddSuffix(pathToStateFile_, ".tmp"sv), pathToStateFile_);

    return true;
}

bool SerializingListener::TryLoadStateFromFile() {
    if (pathToStateFile_.empty()) {
        return false;
    }
    if (!app_) {
        return false;
    }
    serialization::ApplicationRepr repr;
    std::ifstream input_archive{pathToStateFile_};

    InputArchive ar{input_archive};
    ar >> repr;
    repr.Restore(*app_);
    return true;
}

}  // namespace ser_listener
