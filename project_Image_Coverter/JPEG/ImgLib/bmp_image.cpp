#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib_bmp {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    // поля заголовка Bitmap File Header
    uint16_t bfType     = 0x4D42;   // 'BM'
    uint32_t bfSize;                // Size of the file
    uint32_t bfReserved = 0;        // Must be 0
    uint32_t bfOffBits  = 14 + 40;  // Offset to pixel data
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    uint32_t biSize          = 40;          // Size of this header (40 bytes)
    int32_t  biWidth;                       // Width in pixels
    int32_t  biHeight;                      // Height in pixels
    uint16_t biPlanes        = 1;           // Must be 1
    uint16_t biBitCount      = 24;          // Bits per pixel
    uint32_t biCompression   = 0;           // Compression (0 = BI_RGB)
    uint32_t biSizeImage;                   // Image data size
    int32_t  biXPelsPerMeter = 11811;       // Horizontal resolution
    int32_t  biYPelsPerMeter = 11811;       // Vertical resolution
    uint32_t biClrUsed       = 0;           // Colors in color table
    uint32_t biClrImportant  = 0x1000000;   // Important colors
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
// pixels to bytes
static int GetBMPStride(int w) {
    static constexpr int alignment = 4;
    static constexpr int colors = 3;
    return ((w * colors + alignment - 1) / alignment) * alignment;
}


bool SaveBMP(const Path& file, const img_lib::Image& image){
    BitmapFileHeader fileHeader;
    BitmapInfoHeader infoHeader;

    ofstream out(file, ios::binary);
    if (!out) {
        throw std::runtime_error("Failed to open file for writing: " + file.string());
    }
    // pixels to bytes
    const int efWidth = GetBMPStride(image.GetWidth());
    infoHeader.biWidth = image.GetWidth();
    infoHeader.biHeight = image.GetHeight();
    infoHeader.biSizeImage = efWidth * infoHeader.biHeight;

    fileHeader.bfSize = fileHeader.bfOffBits + infoHeader.biSizeImage;


    out.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (!out) {
        throw std::runtime_error("Error writing BMP file header to: " + file.string());
    }
    out.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    if (!out) {
        throw std::runtime_error("Error writing BMP info header to: " + file.string());
    }

    const int w = image.GetWidth();
    const int h = image.GetHeight();
    std::vector<char> buff(efWidth);

    for (int y = h - 1; y >= 0; --y) {
        const img_lib::Color* line = image.GetLine(y);
        for (int x = 0; x < w; ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(buff.data(), buff.size());
    }
    out.flush();
    if (!out.good()) {
        throw runtime_error("Incomplete write to BMP file: " + file.string());
    }

    return true;
}

img_lib::Image LoadBMP(const Path& file){
    BitmapFileHeader fileHeader;
    BitmapInfoHeader infoHeader;
    
    BitmapFileHeader _fileHeader;
    BitmapInfoHeader _infoHeader;

    std::ifstream in(file, std::ios::binary);
    if (!in) 
        return {};
    
    in.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (!in) {
        return {};
    }

    in.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    if (!in) {
        return {};
    }

    if (fileHeader.bfOffBits != _fileHeader.bfOffBits || fileHeader.bfReserved != _fileHeader.bfReserved ||
        fileHeader.bfType != _fileHeader.bfType ) {
        return {};
    }
    if (infoHeader.biSize != _infoHeader.biSize || _infoHeader.biPlanes != infoHeader.biPlanes ||
        infoHeader.biBitCount != _infoHeader.biBitCount) {
        return {};
    }

    const int w = infoHeader.biWidth;
    const int h = infoHeader.biHeight;


    img_lib::Image result(w, h, img_lib::Color::Black());
    std::vector<char> buff(GetBMPStride(w));

    for (int y = h - 1; y >= 0; --y) {
        img_lib::Color* line = result.GetLine(y);
        in.read(buff.data(), buff.size());

        for (int x = 0; x < w; ++x) {
            line[x].b = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].r = static_cast<byte>(buff[x * 3 + 2]);
        }
    }

    return result;
}

}  // namespace img_lib