#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <numeric> // for std::iota
#include <ranges>
#include <concepts>

using namespace std;


vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == '.') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

template <typename It, typename Eq = std::equal_to<>>
bool is_part_of_container(const It& begin1, const It& end1, 
                       const It& begin2, const It& end2, Eq eq = {}) {
    auto it1 = begin1;
    auto it2 = begin2;
    for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
        if (!eq(*it1, *it2)) return false;
    }
    return (it1 == end1) || (it2 == end2); // both must be exhausted
}
    // разработайте класс домена
class Domain {
    vector<string> domain_;
public:
    // конструктор должен позволять конструирование из string, с сигнатурой определитесь сами
    Domain (const string& str)
        : domain_{move(SplitIntoWords(str))}{
            std::reverse(domain_.begin(), domain_.end());
    }
    // разработайте operator==
    bool operator==(const Domain& other) const {
        return domain_ == other.domain_;
    }
    // разработайте метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
    bool IsSubdomain(const Domain& sub) const {
        return is_part_of_container(sub.domain_.begin(), sub.domain_.end(), domain_.begin(), domain_.end());
    }
};

template <typename It>
class DomainChecker {
    const It begin_;
    const It end_;

public:
    // конструктор должен принимать список запрещённых доменов через пару итераторов
    DomainChecker (It begin, It end)
    : begin_{begin}
    , end_{end}
    {};

    // разработайте метод IsForbidden, возвращающий true, если домен запрещён
    bool IsForbidden(const Domain& sub) const {
        for (auto it = begin_; it != end_; it++){
            if (it->IsSubdomain(sub)){
                return true;
            }
        }
        return false; 
    }
};

// разработайте функцию ReadDomains, читающую заданное количество доменов из стандартного входа
template <typename Number>
std::vector<Domain> ReadDomains (istream& input, Number num){
    std::vector<Domain> vec;
    string line;
    for ([[maybe_unused]] auto _: std::views::iota(Number{0}, num)) {
        getline(input, line);
        vec.emplace_back(Domain{line});
    }
    return vec;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    std::getline(input >> std::ws, line);

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
