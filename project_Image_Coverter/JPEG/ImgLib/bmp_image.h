#pragma once
#include "img_lib.h"

#include <filesystem>

namespace img_lib_bmp {
using Path = std::filesystem::path;

bool SaveBMP(const Path& file, const img_lib::Image& image);
img_lib::Image LoadBMP(const Path& file);

} // namespace img_lib
