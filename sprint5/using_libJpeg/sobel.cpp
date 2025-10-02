#include "ppm_image.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string_view>

using namespace std;

int Sum(img_lib::Color *c) {
    return to_integer<int>(c->r) + to_integer<int>(c->g) + to_integer<int>(c->b);
}
img_lib::Image fill(const img_lib::Image& image_){
    auto image = image_;
    auto left = image.GetLine(0);
    auto right = left + image.GetWidth();
    std::fill(left, right, img_lib::Color::Black());
    left = image.GetLine(image.GetHeight() - 1);
    right = left + image.GetWidth();
    std::fill(left, right, img_lib::Color::Black());
    return image;
}

// реализуйте оператор Собеля
img_lib::Image Sobel(const img_lib::Image& image_){
    auto image_change = fill(image_);
    auto image = image_;
    for (int y = 1; y < image.GetHeight() - 1; ++y) {
        auto line = image.GetLine(y);
        auto line_change = image_change.GetLine(y);
        for (int x = 0; x < image.GetWidth(); ++x) {
            if (x == 0){
                line_change[x] = img_lib::Color::Black();
                continue;
            }
            if (x == image.GetWidth() - 1){
                line_change[x] = img_lib::Color::Black();
                continue;
            }
            auto tl = Sum((image.GetLine(y - 1) + x - 1));
            auto tc = Sum((image.GetLine(y - 1) + x - 0));
            auto tr = Sum((image.GetLine(y - 1) + x + 1));
            
            auto cl = Sum(line + x - 1);
            auto cr = Sum(line + x + 1);

            auto bl = Sum((image.GetLine(y + 1) + x - 1));
            auto bc = Sum((image.GetLine(y + 1) + x - 0));
            auto br = Sum((image.GetLine(y + 1) + x + 1));

            
            auto gx = -tl - 2*tc - tr + bl + 2 * bc + br;
            auto gy = -tl - 2*cl - bl + tr + 2 * cr + br;
            auto after_clamp = std::clamp<double>(sqrt(gx*gx + gy*gy), 0.0, 255.0);
            line_change[x].r = static_cast<byte>(after_clamp);
            line_change[x].g = static_cast<byte>(after_clamp);
            line_change[x].b = static_cast<byte>(after_clamp);
        }
    }
    return image_change;
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

    image = Sobel(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}