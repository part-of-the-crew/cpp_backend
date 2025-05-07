// Макрос _USE_MATH_DEFINES необходим, чтобы при подключении <cmath> была объявлена константа M_PI
#define _USE_MATH_DEFINES
// Макрос _USE_MATH_DEFINES следует объявить ДО подключения других заголовочных файлов,
// которые могут подключить <cmath> неявно
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>

using namespace std;

enum class Color { RED, GREEN, BLUE };

ostream& operator<<(ostream& out, Color color) {
    switch (color) {
        case Color::RED:
            out << "red"s;
            break;
        case Color::GREEN:
            out << "green"s;
            break;
        case Color::BLUE:
            out << "blue"s;
            break;
    }
    return out;
}

class Shape {
    Color color_;
    double area_{0.0};
    std::string type_ {"Shape"};
protected:
    Shape(Color color, std::string type) : color_{color}, type_{type} {}
public:
    Shape(Color color) : color_{color} {}

    virtual std::string GetType( void ) const {
        return type_;
    }
    virtual std::string GetType( void ) {
        return type_;
    }
    Color GetColor(void){
        return color_;
    }
    Color GetColor(void) const {
        return color_;
    }
    virtual double GetArea(void){
        return area_;
    }
    virtual double GetArea(void) const {
        return area_;
    }
    void SetColor(Color color){
        color_= color;
    }
};

class Rectangle : public Shape {
    double wigth_;
    double height_;
    std::string type_ {"Rectangle"};
public:
    Rectangle (double wigth, double height, Color color):
        Shape(color), wigth_{wigth}, height_{height}{}

    void SetSize ( double wigth, double height){
        wigth_ = wigth;
        height_ = height;
    }
    double GetWidth(){
        return wigth_;
    }
    double GetHeight(){
        return height_;
    }
    double GetWidth() const {
        return wigth_;
    }
    double GetHeight() const {
        return height_;
    }
    double GetArea(void) override {
        return wigth_*height_;
    }
    double GetArea(void) const override {
        return wigth_*height_;
    }
    std::string GetType ( void ) override {
        return type_;
    }
    std::string GetType ( void ) const override {
        return type_;
    }
};

class Circle : public Shape {
    double radius_;
    std::string type_ {"Circle"};
public:
    Circle (double radius, Color color): Shape(color), radius_{radius}{}
    void SetRadius(double radius){
        radius_ = radius;
    }
    double GetRadius(){
        return radius_;
    }
    double GetRadius() const {
        return radius_;
    }
    double GetArea(void) override {
        return M_PI * radius_ * radius_;
    }
    double GetArea(void) const override {
        return M_PI * radius_ * radius_;
    }
    std::string GetType ( void ) override {
        return type_;
    }
    std::string GetType ( void ) const override {
        return type_;
    }
};

// Возвращает суммарную площадь фигур, указатели на которые находятся в переданной коллекции collection
template <typename ShapeCollection>
double CalcSumArea(const ShapeCollection& collection) {
    double sum = 0;
    for (auto f: collection ){
        sum += f->GetArea();
    }
    return sum;
}

void PrintShapeInfo(const Shape& shape) {
    cout << shape.GetType() << ": color: "s << shape.GetColor() << ", area:"s << shape.GetArea() << endl;
}

int main() {
    Circle c{10.0, Color::RED};
    Rectangle r{10, 20, Color::BLUE};
    Shape sh{Color::GREEN};

    const Shape* shapes[] = {&c, &r, &sh};

    assert(sh.GetType() == "Shape"s);
    assert(c.GetType() == "Circle"s);
    assert(r.GetType() == "Rectangle"s);

    assert(sh.GetColor() == Color::GREEN);
    assert(c.GetColor() == Color::RED);
    assert(r.GetColor() == Color::BLUE);
    sh.SetColor(Color::BLUE);
    c.SetColor(Color::GREEN);
    r.SetColor(Color::RED);
    assert(sh.GetColor() == Color::BLUE);
    assert(c.GetColor() == Color::GREEN);
    assert(r.GetColor() == Color::RED);

    assert(std::abs(r.GetArea() - 200.0) < 1e-5);
    assert(std::abs(c.GetArea() - 314.15) < 1e-2);
    c.SetRadius(1.0);
    assert(std::abs(c.GetArea() - 3.1415) < 1e-4);
    r.SetSize(5, 7);
    assert(r.GetWidth() == 5);
    assert(r.GetHeight() == 7);
    assert(std::abs(r.GetArea() - 35.0) < 1e-5);

    assert(abs(CalcSumArea(shapes) - 38.1416) < 1e-4);

    for (const Shape* shape : shapes) {
        PrintShapeInfo(*shape);
    }
    cout << "Total area: " << CalcSumArea(shapes) << endl;
}