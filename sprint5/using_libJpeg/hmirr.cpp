#include "ppm_image.h"

#include <iostream>
#include <string_view>
//#include <algorithm>

using namespace std;

template<class ForwardIt1, class ForwardIt2>
ForwardIt2 swap_ranges1(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2)
{
    for (; first1 != last1; ++first1, ++first2)
        std::iter_swap(first1, first2);
 
    return first2;
}

// реализуйте горизонтальное отражение
void HMirrInplace(img_lib::Image& image){
    auto high = 0;
    auto low = image.GetHeight() - 1;

    while (high < low) {
        std::swap_ranges(image.GetLine(high), image.GetLine(high) + image.GetWidth(), image.GetLine(low));
        high++;
        low--;
        //std::cout << image.GetLine(high) << " " << image.GetLine(high) + image.GetWidth() << std::endl;
    }
/*
    for (int y = 0; y < image.GetHeight(); ++y) {
        auto high = image.Gethigh(y);
        for (int x = 0; x < image.GetWidth(); ++x) {
            high[x].r = Negate(high[x].r);
            high[x].g = Negate(high[x].g);
            high[x].b = Negate(high[x].b);
        }
    }
    */
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