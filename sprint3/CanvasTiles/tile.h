#pragma once
#include <array>
#include <cassert>

#include "geom.h"

class Tile {
public:
    // Размер тайла 8*8 пикселей.
    constexpr static int SIZE = 8;
    using Image = std::vector<std::string>;
    // Конструктор по умолчанию. Заполняет тайл указанным цветом.
    Tile(char color = ' ') noexcept :
        image_(SIZE, std::string(SIZE, color)) 
        {

        // -------------- не удалять ------------
        assert(instance_count_ >= 0);
        ++instance_count_;  // Увеличиваем счётчик тайлов (для целей тестирования).
        // -------------- не удалять ------------
    }

    Tile(const Tile& other) {
        image_ = other.image_;
        // -------------- не удалять ------------
        assert(instance_count_ >= 0);
        ++instance_count_;  // Увеличиваем счётчик тайлов (для целей тестирования).
        // -------------- не удалять ------------
    }

    ~Tile() {
        // -------------- Не удалять ------------
        --instance_count_;  // Уменьшаем счётчик тайлов.
        assert(instance_count_ >= 0);
        // -------------- не удалять ------------
    }

    /**
     * Изменяет цвет пикселя тайла.
     * Если координаты выходят за пределы тайла, метод ничего не делает.
     */
    void SetPixel(Point p, char color) noexcept {
        if (p.x >= SIZE || p.y >= SIZE)
            return;
        image_.at(p.y).at(p.x) = color;
    }

    /**
     * Возвращает цвет пикселя. Если координаты выходят за пределы тайла, возвращается пробел.
     */
    char GetPixel(Point p) const noexcept {
        if (p.x >= SIZE || p.y >= SIZE || p.x < 0 || p.y < 0)
            return ' ';
        return image_.at(p.y).at(p.x);
    }

    // Возвращает количество экземпляра класса Tile в программе.
    static int GetInstanceCount() noexcept {
        // -------------- не удалять ------------
        return instance_count_;
        // -------------- не удалять ------------
    }

private:
    // -------------- не удалять ------------
    inline static int instance_count_ = 0;
    // -------------- не удалять ------------

    /* Разместите здесь поля для хранения пикселей тайла. */
    Image image_{};
};