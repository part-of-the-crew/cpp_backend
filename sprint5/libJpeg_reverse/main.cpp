#include "ppm_image.h"

#include <iostream>
#include <string_view>
#include <cstdint>
using namespace std;


byte Negate(byte c){
    return std::byte{ static_cast<unsigned char>(255 - std::to_integer<int>(c)) };
}

void NegateInplace(img_lib::Image& image){
    for (int y = 0; y < image.GetHeight(); ++y) {
        auto line = image.GetLine(y);
        for (int x = 0; x < image.GetWidth(); ++x) {
            line[x].r = Negate(line[x].r);
            line[x].g = Negate(line[x].g);
            line[x].b = Negate(line[x].b);
        }
    }
}


int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <input image> <output image>"sv << endl;
        return 1;
    }

    auto image = img_lib::LoadPPM(argv[1]);
    if (!image) {
        cerr << "Error loading image"sv << endl;
        return 2;
    }

    NegateInplace(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}