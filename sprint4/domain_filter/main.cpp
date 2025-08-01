#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <numeric> // for std::iota
#include <ranges>
#include <concepts>
#include <set>

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

    // разработайте класс домена
class Domain {
    vector<string> domain_;

    template <typename It1, typename It2, typename Eq = std::equal_to<>>
        bool is_part_of_container(const It1& begin1, const It1& end1, 
                        const It2& begin2, const It2& end2, Eq eq = {}) const {
        auto it1 = begin1;
        auto it2 = begin2;
        for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
            if (!eq(*it1, *it2)) return false;
        }
        if (it1 == end1 && it2 == end2)
            return true;
        if (it1 == end1)
            return false;
        return true;
    }
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
    bool operator<(const Domain& other) const {
        return domain_ < other.domain_; // lexicographical compare
    }
    // разработайте метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
    bool IsSubdomain(const Domain& sub) const {
        return is_part_of_container(sub.domain_.begin(), sub.domain_.end(), domain_.begin(), domain_.end());
    }
    bool IsDomain(const Domain& dom) const {
        return is_part_of_container(domain_.begin(), domain_.end(), dom.domain_.begin(), dom.domain_.end());
    }
    friend std::ostream& operator<<(std::ostream& os, const Domain& d) {
        // domain_ is reversed; print in normal order
        if (d.domain_.empty()) return os;
        for (size_t i = d.domain_.size(); i-- > 0;) {
            os << d.domain_[i];
            if (i != 0) os << '.';
        }
        return os;
    }
};

template <typename It>
class DomainChecker {
    vector<Domain> banned_domains_;

    bool ExistAsPartOfBannedDomains(const Domain& subdomain) const {
        for (auto const& ee: banned_domains_){
            if (subdomain.IsDomain(ee))
                return true;
        }
        return false;
    }
public:
    // конструктор должен принимать список запрещённых доменов через пару итераторов
    DomainChecker (It begin, It end)
    {
        vector<Domain> banned_domains(begin, end);
        sort(banned_domains.begin(), banned_domains.end());
        for (auto const& e: banned_domains){
            //cout << e << endl;
            if (banned_domains_.empty()){
                banned_domains_.emplace_back(e);
            } else {
                if (!ExistAsPartOfBannedDomains(e)){
                    banned_domains_.emplace_back(e);
                }
            }
        }

    };

    // разработайте метод IsForbidden, возвращающий true, если домен запрещён
    bool IsForbidden(const Domain& sub) const {
        for (auto const& e: banned_domains_){
            if (e.IsSubdomain(sub)){
                return true;
            }
        }
        return false; 
    }

    // разработайте метод IsForbidden, возвращающий true, если домен запрещён
    void Print() const {
        for (auto const& e: banned_domains_){
            cout << e << endl;
        }
    }
    size_t GetSize() const {
        return banned_domains_.size();
    }
};


// разработайте функцию ReadDomains, читающую заданное количество доменов из стандартного входа
template <typename Number>
std::vector<Domain> ReadDomains (istream& input, Number num){
    std::vector<Domain> vec;
    string line;
    for ([[maybe_unused]] auto _: std::views::iota(Number{0}, num)) {
        getline(input, line);
        vec.emplace_back(std::move(line));
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
/*
    Tests
    const std::vector<Domain> v {"com1"s,"example1.com1"s,  "example.com"s, "com"s};
    DomainChecker checker1{v.begin(), v.end()};
    //hecker1.Print();
    //cout << checker1.GetSize() << endl;
*/
}
