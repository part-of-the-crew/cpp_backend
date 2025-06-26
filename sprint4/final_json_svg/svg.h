#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b)
            : red(r)
            , green(g)
            , blue(b) {
    }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba {
    Rgba() = default;
    Rgba(uint8_t r, uint8_t g, uint8_t b, double a)
            : red(r)
            , green(g)
            , blue(b)
            , opacity(a) {
    }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};

//using NoneColor = std::monostate;
using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{};

// Overload for Rgb
inline std::ostream& operator<<(std::ostream& out, const Rgb& color) {
    return out << "rgb(" << static_cast<int>(color.red)
               << "," << static_cast<int>(color.green)
               << "," << static_cast<int>(color.blue) << ")";
}

// Overload for Rgba
inline std::ostream& operator<<(std::ostream& out, const Rgba& color) {
    return out << "rgba(" << static_cast<int>(color.red)
               << "," << static_cast<int>(color.green)
               << "," << static_cast<int>(color.blue)
               << "," << color.opacity << ")";
}

struct OstreamSolutionPrinter {

    std::ostream& out;

    void operator()(std::monostate) const {
        out << "none";
    }
    void operator()(std::string s) const {
        out << s;
    }
    void operator()(svg::Rgb rgb) const {
        out << rgb;
    }
    void operator()(svg::Rgba rgba) const {
        out << rgba;
    }
};

// Overload for Color (std::variant)

inline std::ostream& operator<<(std::ostream& out, const Color& color) {
    std::visit(OstreamSolutionPrinter{out}, color);
    return out;
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};


// Overload operator<< for StrokeLineCap
inline std::ostream& operator<<(std::ostream& os, StrokeLineCap cap) {
    switch (cap) {
        case StrokeLineCap::BUTT:    return os << "butt";
        case StrokeLineCap::ROUND:   return os << "round";
        case StrokeLineCap::SQUARE:  return os << "square";
        default:                     return os << "unknown";
    }
}

// Overload operator<< for StrokeLineJoin
inline std::ostream& operator<<(std::ostream& os, StrokeLineJoin join) {
    switch (join) {
        case StrokeLineJoin::ARCS:        return os << "arcs";
        case StrokeLineJoin::BEVEL:       return os << "bevel";
        case StrokeLineJoin::MITER:       return os << "miter";
        case StrokeLineJoin::MITER_CLIP:  return os << "miter-clip";
        case StrokeLineJoin::ROUND:       return os << "round";
        default:                          return os << "unknown";
    }
}

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }
    Owner& SetStrokeWidth(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        strokeLineCap_ = line_cap;
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        strokeLineJoin_ = line_join;
        return AsOwner();
    }
protected:
    ~PathProps() = default;

    // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << "fill=\""sv << *fill_color_ << "\" "sv;
        }
        if (stroke_color_) {
            out << "stroke=\""sv << *stroke_color_ << "\" "sv;
        }
        if (stroke_width_) {
            out << "stroke-width=\""sv << *stroke_width_ << "\" "sv;
        }
        if (strokeLineCap_) {
            out << "stroke-linecap=\""sv << *strokeLineCap_ << "\" "sv;
        }
        if (strokeLineJoin_) {
            out << "stroke-linejoin=\""sv << *strokeLineJoin_ << "\""sv;
        }
    }

private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> strokeLineCap_;
    std::optional<StrokeLineJoin> strokeLineJoin_;
};


struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

class Object;

class ObjectContainer {
public:
    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }
    // Добавляет в svg-документ объект-наследник svg::Object
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    virtual ~ObjectContainer() = default;
};

// Интерфейс Drawable задаёт объекты, которые можно нарисовать с помощью ObjectContainer
class Drawable {
public:
    virtual void Draw(ObjectContainer& oc) const = 0;
    virtual ~Drawable() = default;
};



/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    virtual void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);
    Circle (Point center, double radius) :
        center_{center},
        radius_{radius}
    {}
    Circle () = default;
private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

    private:
    void RenderObject(const RenderContext& context) const override;

    std::vector<Point> points;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

private:

    // Задаёт координаты опорной точки (атрибуты x и y)
    Point pos_ = {0.0, 0.0};

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Point offset_ = {0.0, 0.0};

    // Задаёт размеры шрифта (атрибут font-size)
    uint32_t size_ = 1;

    // Задаёт название шрифта (атрибут font-family)
    std::string font_family_;

    // Задаёт толщину шрифта (атрибут font-weight)
    std::string font_weight_;

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    std::string data_;

    void RenderObject(const RenderContext& context) const override;
    void Replace (std::string from, std::string to);
};

class Document : public ObjectContainer {
public:

    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

private:

    std::vector <std::unique_ptr<Object>> objects;
};

}  // namespace svg