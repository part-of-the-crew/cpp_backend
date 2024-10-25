#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std::string_literals;

template <typename C>
void PrintRange(const C& begin, const C& end) {
    bool is_first = true;
    for (auto it = begin; it != end; ++it) {
        if (!is_first)
            std::cout << " ";
        is_first = false;
        std::cout << *it;
    }
    std::cout << std::endl;
}

template <typename C>
void FindAndPrint(const C& Container, const int& value) {
    auto it = Container.begin();
    for (; it != Container.end(); ++it) {
        if (*it == value){
           break;
        }
    }
    PrintRange(Container.begin(), it);
    PrintRange(it, Container.end());
}



using namespace std;

int main() {
    set<int> test = {1, 1, 1, 2, 3, 4, 5, 5};
    cout << "Test1"s << endl;
    FindAndPrint(test, 3);
    cout << "Test2"s << endl;
    FindAndPrint(test, 0); // элемента 0 нет в контейнере
    cout << "End of tests"s << endl;
} 