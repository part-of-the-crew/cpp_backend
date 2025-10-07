#pragma once
#include "img_lib.h"

#include <filesystem>

namespace img_lib_jpeg {

using Path = std::filesystem::path;

bool SaveJPEG(const Path& file, const img_lib::Image& image);
img_lib::Image LoadJPEG(const Path& file);

} // of namespace img_lib_jpeg