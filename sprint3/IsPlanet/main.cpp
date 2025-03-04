#include <iostream>
#include <string_view>
#include <vector>
#include <algorithm>

using namespace std;
using namespace std::literals;

const vector<string> v {{
    "Mercury"s, "Venus"s, "Earth"s,
    "Mars"s, "Jupiter"s, "Saturn"s,
    "Uranus"s, "Neptune"s
}};

// clang-format on

bool IsPlanet(string_view name) {
    return std::find(v.begin(), v.end(), name) != v.end();
}

void Test(string_view name) {
    cout << name << " is " << (IsPlanet(name) ? ""sv : "NOT "sv)
        << "a planet"sv << endl;
}

int main() {
    std::string s;
    std::getline(std::cin, s);
    Test(s);
}