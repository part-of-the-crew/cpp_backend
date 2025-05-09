#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

Polyline &Polyline::AddPoint(Point point)
{
    points.push_back(std::move(point));
}

inline Text &Text::SetOffset(Point offset)
{
    offset_ = offset;
}

Text &Text::SetFontSize(uint32_t size)
{
    size_ = size;
}

Text &Text::SetFontFamily(std::string font_family)
{
    font_family_ = font_family;
}

Text &Text::SetFontWeight(std::string font_weight)
{
    font_weight_ = font_weight;
}

Text &Text::SetData(std::string data)
{
    data_ = data;
}

void Circle::RenderObject(const RenderContext &context) const
{
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    out << "/>"sv;
}

}  // namespace svg