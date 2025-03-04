#include <cstdint>
#include <iostream>
#include <optional>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <cassert>
using namespace std;

template <typename T>
std::optional<T> SafeAdd(T a, T b) {
    if (a > 0 && b > 0) {
        if (a > numeric_limits<T>::max() - b) {
            return nullopt;
        }
    }
    if (a < 0 && b < 0) {
        if (a < numeric_limits<T>::min() - b) {
            return nullopt;
        }
    }
    return a + b;
}

int main() {
    int64_t a;
    int64_t b;
    cin >> a >> b;
    auto result = SafeAdd(a, b);
    if (result.has_value()) {
        cout << result.value() << endl;
    } else {
        cout << "Overflow!" << endl;
    }
}