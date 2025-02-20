#pragma once
#include "texture.h"
#include <iostream>
#include <memory>

// Поддерживаемые виды фигур: прямоугольник и эллипс
enum class ShapeType { RECTANGLE, ELLIPSE};

class Shape {
public:
    // Фигура после создания имеет нулевые координаты и размер,
    // а также не имеет текстуры
    explicit Shape(ShapeType type)
        : type_(type)
        , pos_({0, 0})
        , size_({0, 0})
        , texture_(nullptr)
    {
    }

    void SetPosition(Point pos) {
        pos_ = pos;
        // Заглушка. Реализуйте метод самостоятельно
    }

    void SetSize(Size size) {
        size_ = size;
        // Заглушка. Реализуйте метод самостоятельно
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        texture_ = texture;
        // Заглушка. Реализуйте метод самостоятельно
    }

    // Рисует фигуру на указанном изображении
	// В зависимости от типа фигуры должен рисоваться либо эллипс, либо прямоугольник
    // Пиксели фигуры, выходящие за пределы текстуры, а также в случае, когда текстура не задана,
    // должны отображаться с помощью символа точка '.'
    // Части фигуры, выходящие за границы объекта image, должны отбрасываться.
    void Draw(Image& image) const {
        // Заглушка. Реализуйте метод самостоятельно
        /*
        2.  как раз в методе Draw нужно и размещать фигуру на полотно - 
        и выбирать какие цвета из текстуры брать. 
        Нужно циклами пройтись по всем возможным точкам :
        for (int y = 0; y < size_.height; ++y) {
            for (int x = 0; x < size_.width; ++x)
        и рассматривать расположение точек {x, y} и {x + pos_.x, y + pos_.y} - расположены ли они внутри фигуры? 
        внутри полотна? внутри текстуры? - и в зависимости от этого уже определять какой именно цвет 
        присваивать image в точке {x + pos_.x, y + pos_.y}
        1 this image to draw
        2 image in texture

        */
        Size tsize;
        if (!texture_) {
            tsize = {0,0};
        } else {
            tsize = texture_->GetSize();
        }
        
        auto isize = GetImageSize(image);
        //if (isize.height == 0 || isize.width == 0 || tsize.height == 0 || tsize.width == 0)
        //    return;

        //auto pixel_color = texture_->GetPixelColor({0, 0});
        // Заполняем полотно цветом фигуры
        //std::cout << pos_.x << " " << pos_.y << texture_->GetPixelColor({pos_.x, pos_.y}) << std::endl;
        //for (int y = 0; y < size_.height; ++y) {
            //for (int x = 0; x < size_.width; ++x) {
        for (int y = pos_.y; y < std::min(size_.height + pos_.y, isize.height); ++y) {
            for (int x = pos_.x; x < std::min(size_.width + pos_.x, isize.width); ++x) {
                //image.at(y).at(x) =  ' ';
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
    std::shared_ptr<Texture> texture_ = nullptr;  // Указатель на текстуру, может быть nullptr
};