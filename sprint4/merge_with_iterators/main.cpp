#include <iostream>
#include <list>
#include <set>
#include <string_view>
#include <vector>
#include <span>
#include <functional>
using namespace std;

// Измените сигнатуру этой функции, чтобы она могла
// сливать не только векторы, но и любые другие контейнеры
template <typename InputIt1, typename InputIt2>
void Merge(InputIt1 src1_1, InputIt1 src1_2, 
           InputIt2 src2_1, InputIt2 src2_2, ostream& out) {
    std::less<> less; // works with different types

    while (src1_1 != src1_2 && src2_1 != src2_2) {
        if (less(*src1_1, *src2_1)) {
            out << *src1_1++ << '\n';
        } else {
            out << *src2_1++ << '\n';
        }
    }
    while (src1_1 != src1_2) out << *src1_1++ << '\n';
    while (src2_1 != src2_2) out << *src2_1++ << '\n';
}

template <typename Container>
auto make_span(Container& c) -> std::span<typename Container::value_type> {
    return std::span<typename Container::value_type>(c.data(), c.size());
}
template <typename Container>
auto make_span(const Container& c) -> std::span<const typename Container::value_type> {
    return std::span<const typename Container::value_type>(c.data(), c.size());
}
template <typename T>
void MergeSomething(const vector<T>& src1, const vector<T>& src2, ostream& out) {
    Merge(src1.begin(), src1.end(), 
          src2.begin(), src2.end(), out);
}

// Реализуйте эти функции. Они должны вызывать ваш вариант функции Merge.
// Чтобы сократить кличество работы, можете реализовать вместо них одну шаблонную.
template <typename T, typename S>
void MergeSomething(const T& src1, const S& src2, ostream& out) {
    Merge(src1.begin(), src1.end(), 
          src2.begin(), src2.end(), out);
}

// Реализуйте эту функцию:
template <typename T>
void MergeHalves(const vector<T>& src, ostream& out) {
    size_t mid = (src.size() + 1) / 2;
    // Сделать Merge участка вектора от 0 до mid и от mid до конца.
    // Элемент с индексом mid включается во второй диапазон.
    Merge(src.begin(), src.begin() + mid, src.begin() + mid, src.end(), out);
}

int main() {
    vector<int> v1{60, 70, 80, 90};
    vector<int> v2{65, 75, 85, 95};
    vector<int> combined{60, 70, 80, 90, 65, 75, 85, 95};
    list<double> my_list{0.1, 72.5, 82.11, 1e+30};
    string_view my_string = "ACNZ"sv;
    set<unsigned> my_set{20u, 77u, 81u};

    // пока функция MergeSomething реализована пока только для векторов
    cout << "Merging vectors:"sv << endl;
    MergeSomething(v1, v2, cout);

    cout << "Merging vector and list:"sv << endl;
    MergeSomething(v1, my_list, cout);

    cout << "Merging string and list:"sv << endl;
    MergeSomething(my_string, my_list, cout);

    cout << "Merging set and vector:"sv << endl;
    MergeSomething(my_set, v2, cout);

    cout << "Merging vector halves:"sv << endl;
    MergeHalves(combined, cout);
}