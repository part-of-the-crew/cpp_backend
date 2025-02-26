#pragma once

#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "cow.h"
#include "geom.h"
#include "tile.h"

class Image {
public:
    /**
     * Конструирует изображение заданного размера. Если размеры не являются положительными,
     * выбрасывает исключение std::out_of_range.
     */
    explicit Image(Size size, char color = ' ')
        : size_{size.width > 0 && size.height > 0 ? size
                                                  : throw std::out_of_range("Invalid image size")}
        , size_in_tiles_{(size_.width + Tile::SIZE - 1) / Tile::SIZE,
                         (size_.height + Tile::SIZE - 1) / Tile::SIZE}
        , tiles_{static_cast<size_t>(size_in_tiles_.width * size_in_tiles_.height),
            CoW<Tile>{Tile{color}}} {
    }

    // Возвращает размер изображения в пикселях.
    Size GetSize() const noexcept {
        return size_;
    }

    /**
     * Возвращает «цвет» пикселя в указанных координатах.Если координаты выходят за пределы
     * изображения, возвращает «пробел».
     */
    char GetPixel(Point p) const noexcept {
        if (!IsPointInSize(p, size_))
            return ' ';
        return tiles_[GetTileIndex(p)]->GetPixel(GetTilePoint(p));
    }

    /**
     * Задаёт «цвет» пикселя в указанных координатах. Если координаты выходят за пределы изображения
     * действие игнорируется.
     */
    void SetPixel(Point p, char color) {
        if (!IsPointInSize(p, size_))
            return;
        //tiles_[GetTileIndex(p)]->SetPixel(GetTilePoint(p), color);
        auto& tile = tiles_[GetTileIndex(p)].WriteBad();
        tile.SetPixel(GetTilePoint(p), color);
    }

private:
    //using CoWTile = CoW<Tile>;

    // Возвращает порядковый номер тайла, в котором находится пиксель с координатами p
    int GetTileIndex(Point p) const noexcept {
        assert((p.x >= 0) && (p.x < size_.width) && (p.y >= 0) && (p.y < size_.height));
        return (p.y / Tile::SIZE) * size_in_tiles_.width + (p.x / Tile::SIZE);
    }
    // Возвращает координаты внутри тайла, в котором находится пиксель с координатами p
    Point GetTilePoint(Point p) const noexcept {
        return {p.x % Tile::SIZE, p.y % Tile::SIZE};
    }
    
    Size size_;                   // Размер изображения в пикселях.
    Size size_in_tiles_;          // Размер изображения в тайлах.
    std::vector<CoW<Tile>> tiles_;  // Тайлы изображения (строка за строкой).
};

/**
 * Выводит в поток out изображение в виде символов.
 */
inline void Print(const Image& img, std::ostream& out) {
    const auto size = img.GetSize();
    for (int y = 0; y < size.height; ++y) {
        for (int x = 0; x < size.width; ++x) {
            out.put(img.GetPixel({x, y}));
        }
        out.put('\n');
    }
}

/**
 * Загружает изображение из pixels. Линии изображения разделяются символами \n.
 * Размеры картинки определяются по количеству переводов строки и самой длинной линии.
 */
inline Image LoadImage(const std::string& pixels) {
    std::istringstream s(pixels);
    Size size;
    std::string line;
    while (std::getline(s, line)) {
        size.width = std::max(size.width, static_cast<int>(line.length()));
        ++size.height;
    }

    Image img(size);

    s = std::istringstream(pixels);
    for (int y = 0; y < size.height; ++y) {
        if (!std::getline(s, line)) break;

        int x = 0;
        for (char ch : line) {
            img.SetPixel({x++, y}, ch);
        }
    }

    return img;
}