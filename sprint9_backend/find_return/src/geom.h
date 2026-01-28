#pragma once

namespace geom {

using Dimension = int;
using Coord = Dimension;

struct Point2D {
    Coord x, y;
};
struct Size {
    Dimension width, height;
};
struct Rectangle {
    Point2D position;
    Size size;
};
struct Offset {
    Dimension dx, dy;
};

// Координаты в вещественных числах
struct Position {
    double x = 0.0;
    double y = 0.0;
};
struct Speed {
    double ux = 0.0;
    double uy = 0.0;
};

// Направление
enum class Direction { NORTH, SOUTH, WEST, EAST };

Point2D ToPoint2D(const Position& p) {
    return {p.x, p.y};
}

// -----------------------
}  // namespace geom
