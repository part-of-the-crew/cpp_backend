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
        circles.emplace_back(Circle{{center.x, center.y + rad*5}, rad*2});
        circles.emplace_back(Circle{{center.x, center.y + rad*2}, rad*1.5});
        circles.emplace_back(Circle{center, rad});
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

int main() {
    using namespace svg;
    using namespace shapes;
    using namespace std;
    //Polymorph();
    //std::cout << "ObjectContainer is abstract: " << !std::is_abstract_v<ObjectContainer> << '\n';
    //assert(!std::is_abstract_v<ObjectContainer> == 1);
    vector<unique_ptr<svg::Drawable>> picture;
    Circle c;
    picture.emplace_back(make_unique<Triangle>(Point{100, 20}, Point{120, 50}, Point{80, 40}));
    // 5-лучевая звезда с центром {50, 20}, длиной лучей 10 и внутренним радиусом 4
    picture.emplace_back(make_unique<Star>(Point{50.0, 20.0}, 10.0, 4.0, 5));
    // Снеговик с "головой" радиусом 10, имеющей центр в точке {30, 20}
    picture.emplace_back(make_unique<Snowman>(Point{30, 20}, 10.0));

    svg::Document doc;
    // Так как документ реализует интерфейс ObjectContainer,
    // его можно передать в DrawPicture в качестве цели для рисования
    DrawPicture(picture, doc);

    // Выводим полученный документ в stdout
    doc.Render(cout);
}