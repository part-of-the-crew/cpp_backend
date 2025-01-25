#include <iostream>
#include <string>

using namespace std;

struct Circle {
    double x;
    double y;
    double r;
};

struct Dumbbell {
    Circle circle1;
    Circle circle2;
    string text;
};

struct DumbbellHash {
    size_t operator()(const Dumbbell& d) const {
        //return getCircleHash(d.circle1) ^ getCircleHash(d.circle2)*3 ^ std::hash<string>{}(d.text);
        return getCircleHash(d.circle1) ^ getCircleHash(d.circle2)*3 ^ s_hasher(d.text);
    }
    size_t getCircleHash(const Circle& c) const {
        return bit_cast<size_t>(c.x) ^ (bit_cast<size_t>(c.y) >> 1) ^ (bit_cast<size_t>(c.r) >> 2);
    }
    hash<string> s_hasher;
};

int main() {
    DumbbellHash hash;
    Dumbbell dumbbell{{10, 11.5, 2.3}, {3.14, 15, -8}, "abc"s};
    cout << "Dumbbell hash "s << hash(dumbbell);
}