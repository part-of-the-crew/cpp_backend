#include "ppm_image.h"

#include <algorithm>
#include <iostream>
#include <string_view>

using namespace std;

// реализуйте вертикальное отражение
void HMirrInplace(img_lib::Image& image){
    for (int y = 0; y < image.GetHeight(); ++y) {
        auto left = image.GetLine(y);
        auto right = image.GetLine(y) + image.GetWidth() - 1;
        while (left < right)
        {
            std::swap(*left, *right);
            left++;
            right--;
            //std::cout << image.GetLine(high) << " " << image.GetLine(high) + image.GetWidth() << std::endl;
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

    HMirrInplace(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}