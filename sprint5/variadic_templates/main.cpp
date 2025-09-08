#include <cassert>
#include <string>
#include <string_view>
#include <memory>

using namespace std::literals;
/* Напишите вашу реализацию EqualsToOneOf здесь*/


// Решение, использующее рекурсию
/*
template <typename T>
bool EqualsToOneOf(const T&) {
    return false;
}
template <typename T, typename U, typename... Other>
bool EqualsToOneOf(const T& x, const U& y, const Other&... other) {
    return (x == y) || EqualsToOneOf(x, other...);
}
*/



template <typename T0, typename... Ts>
bool EqualsToOneOfImpl(const T0& v0, const Ts&... vs) {
    return (...|| (v0 == vs));
}

// Функция Log остаётся без изменений
template <typename... Ts>
bool EqualsToOneOf(const Ts&... vs) {
    if constexpr (sizeof...(vs) < 2) {
       return false;
    }
    return EqualsToOneOfImpl(vs...);
} 

// Шаблон ApplyToMany применяет функцию f (первый аргумент) последовательно к каждому из остальных своих аргументов
template <typename T, typename... Ts>
void ApplyToMany(T& func, Ts&&... vs) {
    (func(std::forward<Ts>(vs)), ...); 
}


void TestSum() {
    int x;
    auto lambda = [&x](int y) {
        x += y;
    };

    x = 0;
    ApplyToMany(lambda, 1);
    assert(x == 1);

    x = 0;
    ApplyToMany(lambda, 1, 2, 3, 4, 5);
    assert(x == 15);
}

void TestConcatenate() {
    using namespace std::literals;
    std::string s;
    auto lambda = [&s](const auto& t) {
        if (!s.empty()) {
            s += " ";
        }
        s += t;
    };

    ApplyToMany(lambda, "cyan"s, "magenta"s, "yellow"s, "black"s);
    assert(s == "cyan magenta yellow black"s);
}

void TestIncrement() {
    auto increment = [](int& x) {
        ++x;
    };

    int a = 0;
    int b = 3;
    int c = 43;

    ApplyToMany(increment, a, b, c);
    assert(a == 1);
    assert(b == 4);
    assert(c == 44);
}

void TestArgumentForwarding() {
    struct S {
        int call_count = 0;
        int i = 0;
        std::unique_ptr<int> p;
        void operator()(int i) {
            this->i = i;
            ++call_count;
        }
        void operator()(std::unique_ptr<int>&& p) {
            this->p = std::move(p);
            ++call_count;
        }
    };

    S s;

    ApplyToMany(s, 1, std::make_unique<int>(42));
    assert(s.call_count == 2);
    assert(s.i == 1);
    assert(s.p != nullptr && *s.p == 42);
}

void TestArgumentForwardingToConstFunction() {
    struct S {
        mutable int call_count = 0;
        mutable int i = 0;
        mutable std::unique_ptr<int> p;
        void operator()(int i) const {
            this->i = i;
            ++call_count;
        }
        void operator()(std::unique_ptr<int>&& p) const {
            this->p = std::move(p);
            ++call_count;
        }
    };

    const S s;
    ApplyToMany(s, 1, std::make_unique<int>(42));
    assert(s.call_count == 2);
    assert(s.i == 1);
    assert(s.p != nullptr && *s.p == 42);
}




int main() {
    assert(EqualsToOneOf("hello"sv, "hi"s, "hello"s));
    assert(!EqualsToOneOf(1, 10, 2, 3, 6));
    assert(!EqualsToOneOf(8));

    TestSum();
    TestConcatenate();
    TestIncrement();
    TestArgumentForwarding();
    TestArgumentForwardingToConstFunction();
}
