#include <cassert>
#include <string>
#include <iostream>
#include <stdexcept>
#include <exception>

using namespace std;

class House {
    const int length = 0;
    const int width = 0;
    const int height = 0;

    public:
    House (int l, int w, int h): length{l}, width{w}, height{h} {};
    int GetLength(){
        return length;
    }
    int GetWidth(){
        return width;
    }
    int GetHeight(){
        return height;
    }
    int GetLength() const {
        return length;
    }
    int GetWidth() const {
        return width;
    }
    int GetHeight() const {
        return height;
    }
};

class Resources {
    int bricks_;
    public:
    Resources (int bricks): bricks_{bricks} {};
    int GetBrickCount(){
        return bricks_;
    }
    int GetBrickCount() const {
        return bricks_;
    }
    void TakeBricks (int bricks){
        if (bricks > bricks_ || bricks < 0)
            throw std::out_of_range{"invalid"};

        bricks_ -= bricks;
    }
};

struct HouseSpecification {
    int length = 0;
    int width = 0;
    int height = 0;
};

class Builder {
    Resources &resources;

    public:

    Builder (Resources &res):resources{res} {};

    House BuildHouse(HouseSpecification hs){
        int bricks_needed = 2 * (hs.width * 4 * hs.height * 8) + 2 * (hs.length * 4 * hs.height * 8);
        try {
            resources.TakeBricks (bricks_needed);
        } catch (...) {
            throw std::runtime_error{"invalid"};
        }
        return {hs.length, hs.width, hs.height};
    }

};

int main() {
    Resources resources{10000};
    Builder builder1{resources};
    Builder builder2{resources};

    House house1 = builder1.BuildHouse(HouseSpecification{12, 9, 3});
    assert(house1.GetLength() == 12);
    assert(house1.GetWidth() == 9);
    assert(house1.GetHeight() == 3);
    cout << resources.GetBrickCount() << " bricks left"s << endl;

    House house2 = builder2.BuildHouse(HouseSpecification{8, 6, 3});
    assert(house2.GetLength() == 8);
    cout << resources.GetBrickCount() << " bricks left"s << endl;

    House banya = builder1.BuildHouse(HouseSpecification{4, 3, 2});
    assert(banya.GetHeight() == 2);
    cout << resources.GetBrickCount() << " bricks left"s << endl;
}