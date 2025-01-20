#include <iostream>
#include <string>
#include <string_view>
#include <ranges>
#include <vector>
#include <type_traits>
#include <queue>
#include <algorithm>
#include <iterator>
#include <map>
#include <unordered_map>
#include <tuple>

#include "log_duration.h"

template<typename T>
concept BasicString = std::is_same_v<T, std::basic_string<typename T::value_type>>;

template<typename T>
concept StringLike = BasicString<T> || std::is_convertible_v<T, std::string>;

template<typename T>
concept range = std::is_class_v<T> && !StringLike<T>;

template <range C>
void print(const C& s) {
    for (const auto& e : s)
        std::cout << e << " ";
    std::cout << std::endl;
}


template <typename C> 
void print(const C& s) {
    std::cout << s << "";
    std::cout << std::endl;
}

using namespace std;

vector<pair<string, int>> GetSortedWordCounts(vector<string> words) {
    unordered_map<string, int> counts_map;

    {
        LOG_DURATION ("Filling");
        for (auto& word : words) {
            ++counts_map[move(word)];
        }
    }
    vector<pair<string, int>> counts_vector(move_iterator(counts_map.begin()), move_iterator(counts_map.end()));
    sort(counts_vector.begin(), counts_vector.end(), [](const auto& l, const auto& r) {
        return l.second > r.second;
    });

    return counts_vector;
}

int main() {
    vector<string> words;
    string word;

    while (cin >> word) {
        words.push_back(word);
    }

    auto counts_vector = GetSortedWordCounts(move(words));

    cout << "Word"s << endl;

    for (auto [iter, i] = tuple(counts_vector.begin(), 0); i < 10 && iter != counts_vector.end(); ++i, ++iter) {
        cout << iter->first << " - "s << iter->second << endl;
    }
}

std::vector<std::string_view> SplitIntoWordsView(std::string_view str) {
    std::vector<std::string_view> result;
    str.remove_prefix(std::min(str.find_first_not_of(" "), str.size()));
    
    while (!str.empty()) {
        int64_t space = str.find(' ');
        result.push_back(str.substr(0, space));
        str.remove_prefix(std::min(str.find_first_not_of(" ", space), str.size()));
    }

    return result;
}


