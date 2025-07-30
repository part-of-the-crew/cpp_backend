#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;

class Domain {
    // разработайте класс домена

    // конструктор должен позволять конструирование из string, с сигнатурой определитесь сами

    // разработайте operator==

    // разработайте метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
};

class DomainChecker {
public:
    // конструктор должен принимать список запрещённых доменов через пару итераторов

    // разработайте метод IsForbidden, возвращающий true, если домен запрещён
};

// разработайте функцию ReadDomains, читающую заданное количество доменов из стандартного входа

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

int main() {
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
