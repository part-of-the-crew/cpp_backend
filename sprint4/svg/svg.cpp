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
    return *this;
}

Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

Text &Text::SetOffset(Point offset)
{
    offset_ = offset;
    return *this;
}

Text &Text::SetFontSize(uint32_t size)
{
    size_ = size;
    return *this;
}

Text &Text::SetFontFamily(std::string font_family)
{
    font_family_ = font_family;
    return *this;
}

Text &Text::SetFontWeight(std::string font_weight)
{
    font_weight_ = font_weight;
    return *this;
}

Text &Text::SetData(std::string data)
{
    data_ = data;
    return *this;
}

void Circle::RenderObject(const RenderContext &context) const
{
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    out << "/>"sv;
}

void Polyline::RenderObject(const RenderContext &context) const
{
    //<polyline points="20,40 22.9389,45.9549 29.5106,46.9098 24.7553,51.5451.." />
    context.out << "<polyline points=\""sv;
    bool first = true;
    for (auto p: points){
        if (first)
            context.out << " ";
        context.out << p.x << "," << p.y;
        first = false;
    }
    context.out << "\" />"sv;
}

void Text::RenderObject(const RenderContext &context) const
{
    //<text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana" font-weight="bold">Hello C++</text>
    context.out << "<text "sv;
    context.out << "x=\"" << pos_.x << "\" "sv;
    context.out << "y=\"" << pos_.y << "\" "sv;
    context.out << "dx=\"" << offset_.x << "\" "sv;
    context.out << "dy=\"" << offset_.y << "\" "sv;
    context.out << "font-size=\"" << size_ << "\" "sv;
    context.out << "font-family=\"" << font_family_ << "\" "sv;
    context.out << "font-weight=\"" << font_weight_<< "\" "sv;
    context.out << ">" << data_ << "</text>"sv;
}

void Document::AddPtr(std::unique_ptr<Object> &&obj)
{
    objects.emplace_back(std::move(obj));
}

void Document::Render(std::ostream &out) const
{
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    
    
    for (const auto &obj: objects){
        obj->Render(out);
    }
    out << "</svg>"sv << std::endl;
}

} // namespace svg