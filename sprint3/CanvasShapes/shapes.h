#pragma once
#include "texture.h"
#include <iostream>
#include <memory>

enum class ShapeType { RECTANGLE, ELLIPSE};

class Shape {
public:

    explicit Shape(ShapeType type)
        : type_(type)
        , pos_({0, 0})
        , size_({0, 0})
        , texture_(nullptr)
    {
    }

    void SetPosition(Point pos) {
        pos_ = pos;
    }

    void SetSize(Size size) {
        size_ = size;
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        texture_ = texture;
    }

    void Draw(Image& image) const {
        Size tsize;
        if (!texture_) {
            tsize = {0,0};
        } else {
            tsize = texture_->GetSize();
        }
        
        auto isize = GetImageSize(image);
        for (int y = pos_.y; y < std::min(size_.height + pos_.y, isize.height); ++y) {
            for (int x = pos_.x; x < std::min(size_.width + pos_.x, isize.width); ++x) {
                if (type_ == ShapeType::RECTANGLE) {
                    if (tsize.height <= y - pos_.y || tsize.width <= x - pos_.x) {
                        image.at(y).at(x) = '.';
                    } else {
                        image.at(y).at(x) = texture_->GetPixelColor({x - pos_.x, y - pos_.y});
                    }
                } else if (type_ == ShapeType::ELLIPSE) {
                    if (IsPointInEllipse(Point{x - pos_.x, y - pos_.y}, size_)) {
                        if (tsize.height <= y - pos_.y || tsize.width <= x - pos_.x) {
                            image.at(y).at(x) = '.';
                        } else {
                            image.at(y).at(x) = texture_->GetPixelColor({x - pos_.x, y - pos_.y});
                        }
                    }
                }
            }
        }
    }

    //
private:
    ShapeType type_;
    Point pos_ = {};
    Size size_ = {};
    std::shared_ptr<Texture> texture_ = nullptr;
};