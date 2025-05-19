#define _USE_MATH_DEFINES
#include "svg.h"
#include <cassert>
#include <cmath>

using namespace std::literals;
using namespace svg;

namespace {
svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
    using namespace svg;
    Polyline polyline;
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays) {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    return polyline;
}
}

namespace shapes {

class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {
    }

    // Реализует метод Draw интерфейса svg::Drawable
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }

private:
    svg::Point p1_, p2_, p3_;
};
    
class Star : public svg::Drawable {
public:
    Star(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
        star = CreateStar(center, outer_rad, inner_rad, num_rays);
        star.SetFillColor("red").SetStrokeColor("black");
    }

    void Draw(svg::ObjectContainer& container) const override {
        container.Add(star);
    }

private:
    svg::Polyline star;
};


class Snowman : public svg::Drawable {
public:
    Snowman(svg::Point center, double rad) {
        circles.emplace_back(Circle{{center.x, center.y + rad*5}, rad*2}.
            SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        circles.emplace_back(Circle{{center.x, center.y + rad*2}, rad*1.5}
            .SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        circles.emplace_back(Circle{center, rad}
            .SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
    }

    void Draw(svg::ObjectContainer& container) const override {
        for (auto& circle :circles){
            container.Add(circle);
        }
    }

private:
    std::vector <Circle> circles;
};
    
} // namespace shapes




template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

void Polymorph() {
    using namespace std;
        {
            Circle c;
            Object& obj = c;
 
            obj.Render(cout);
            cout << endl;
        } {
            Polyline p;
            Object& obj = p;
 
            obj.Render(cout);
            cout << endl;
        } {
            Text t;
            Object& obj = t;
 
            obj.Render(cout);
        }
    }

// Выполняет линейную интерполяцию значения от from до to в зависимости от параметра t
uint8_t Lerp(uint8_t from, uint8_t to, double t) {
    return static_cast<uint8_t>(std::round((to - from) * t + from));
}

// Выполняет линейную интерполяцию Rgb цвета от from до to в зависимости от параметра t
svg::Rgb Lerp(svg::Rgb from, svg::Rgb to, double t) {
    return {Lerp(from.red, to.red, t), Lerp(from.green, to.green, t), Lerp(from.blue, to.blue, t)};
}

int main() {
    using namespace svg;
    using namespace std;

    svg::Color color1;                               // none
    svg::Color color2 = svg::Rgb{215, 30, 25};       // rgb(215,30,25)
    svg::Color color3 = svg::NoneColor;              // none
    svg::Color color4 = svg::Rgba{15, 15, 25, 0.7};  // rgba(15,15,25,0.7)
    svg::Color color5 = "red"s;                      // red
    svg::Color color6 = svg::Rgb{};                  // rgb(0,0,0)
    svg::Color color7 = svg::Rgba{};                 // rgba(0,0,0,1.0); 

    Color none_color;
    cout << none_color << endl; // none
    
    Color purple{"purple"s};
    cout << purple << endl; // purple
    
    Color rgb_ = Rgb{100, 200, 255};
    cout << rgb_ << endl; // rgb(100,200,255)

    Color rgba_ = Rgba{100, 200, 255, 0.5};
    cout << rgba_ << endl; // rgba(100,200,255,0.5)

/*
    svg::Rgb rgb{255, 0, 100};
    
    assert(rgb.red == 255);
    assert(rgb.green == 0);
    assert(rgb.blue == 100);

    // Задаёт цвет по умолчанию: red=0, green=0, blue=0
    svg::Rgb color;
    assert(color.red == 0 && color.green == 0 && color.blue == 0); 


    svg::Rgba rgba{100, 20, 50, 0.3};
    assert(rgba.red == 100);
    assert(rgba.green == 20);
    assert(rgba.blue == 50);
    assert(rgba.opacity == 0.3);
    {
        // Чёрный непрозрачный цвет: red=0, green=0, blue=0, alpha=1.0
        svg::Rgba color;
        assert(color.red == 0 && color.green == 0 && color.blue == 0 && color.opacity == 1.0); 
    }
*/
    Circle c;
    c.SetRadius(3.5).SetCenter({1.0, 2.0});
    c.SetFillColor(rgba_);
    c.SetStrokeColor(purple);
    
    Document doc;
    doc.Add(std::move(c));
    doc.Render(cout);
} 