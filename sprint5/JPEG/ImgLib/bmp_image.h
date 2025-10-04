#pragma once
#include "img_lib.h"

#include <filesystem>

namespace img_lib_bmp {
using Path = std::filesystem::path;

bool SavePPM(const Path& file, const img_lib::Image& image);
img_lib::Image LoadPPM(const Path& file);

} // namespace img_lib
