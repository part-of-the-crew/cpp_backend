#pragma once
#include "common.h"

class Texture {
public:
    explicit Texture(Image image)
        : image_(std::move(image)) {
    }

    Size GetSize() const {
        return GetImageSize(image_);
    }

    char GetPixelColor(Point p) const {
        
        if (p.x < 0 || p.x >= static_cast<int>(image_.front().size()) ||
            p.y < 0 || p.y >= static_cast<int>(image_.size())) {
            return ' ';
        }
        
        return image_[p.y][p.x];
    }

private:
    Image image_;
};