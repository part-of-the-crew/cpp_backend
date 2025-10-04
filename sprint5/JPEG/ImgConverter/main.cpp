#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>

#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;



enum class Format {
    JPEG,
    PPM,
    BMP,
    UNKNOWN
};

Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    if (ext == ".bmp"sv) {
        return Format::BMP;
    }

    return Format::UNKNOWN;
}

namespace FormatInterfaces {

class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

class JPEG : public ImageFormatInterface {
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib_jpeg::SaveJPEG(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib_jpeg::LoadJPEG(file);
    }
};

class PPM : public ImageFormatInterface {
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib_ppm::SavePPM(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib_ppm::LoadPPM(file);;
    }
};

class BMP : public ImageFormatInterface {
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib_bmp::SavePPM(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib_bmp::LoadPPM(file);;
    }
};

} //namespace FormatInterfaces {

const FormatInterfaces::ImageFormatInterface* GetFormatInterface(const img_lib::Path& path){
    static const FormatInterfaces::PPM ppmInterface;
    static const FormatInterfaces::JPEG jpegInterface;
    static const FormatInterfaces::BMP bmpInterface;

    auto format = GetFormatByExtension (path);
    if (format == Format::JPEG){
        return &jpegInterface;
    }
    if (format == Format::PPM){
        return &ppmInterface;
    }
    if (format == Format::BMP){
        return &bmpInterface;
    }
    return nullptr;
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];

    auto pInterfaceIn = GetFormatInterface (in_path);
    if (pInterfaceIn == nullptr) {
        cerr << "Unknown format of the input file"sv << endl;
        return 2;
    }


    auto pInterfaceOut = GetFormatInterface (out_path);
    if (pInterfaceOut == nullptr) {
        cerr << "Unknown format of the outnput file"sv << endl;
        return 3;
    }

    img_lib::Image image = pInterfaceIn->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }

    if (!pInterfaceOut->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }

    cout << "Successfully converted"sv << endl;
}