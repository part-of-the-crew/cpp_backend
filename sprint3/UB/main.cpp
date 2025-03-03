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
    // при переполнении возвращается nullopt, иначе - сумма
    ...
    std::numeric_limits<T>::max();
}

int main() {
    int64_t a;
    int64_t b;
    cin >> a >> b;
    cout << a + b << endl;
}