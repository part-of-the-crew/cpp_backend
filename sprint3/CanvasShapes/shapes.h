#pragma once
#include "texture.h"

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
        if (!texture_)
            return;
        auto tsize = texture_->GetSize();
        auto pixel_color = texture_->GetPixelColor({0, 0});
        for (int y = 0; y < size_.height; ++y) {
            for (int x = 0; x < size_.width; ++x) {
                if (type_ == ShapeType::RECTANGLE) {
                    if (IsPointInEllipse(Point{x, y}, size_)) {
                        pixel_color = texture_->GetPixelColor({x + pos_.x, y + pos_.y});
                    } else {

                    }
                } else if (type_ == ShapeType::ELLIPSE) {
                    if (IsPointInEllipse(Point{x, y}, size_)) {
                        pixel_color = texture_->GetPixelColor({x + pos_.x, y + pos_.y});
                    } else {

                    }
                }
            }
        }
    }

    //
private:
    ShapeType type_;
    Point pos_;
    Size size_;
    std::shared_ptr<Texture> texture_;  // Указатель на текстуру, может быть nullptr
};