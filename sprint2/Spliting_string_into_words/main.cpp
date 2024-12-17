#include <cassert>
#include <iostream>
#include <string_view>
#include <vector>

using namespace std;

vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result;
    str.remove_prefix(min(str.find_first_not_of(" "), str.size()));
    
    while (!str.empty()) {
        // тут выполнен инвариант: str не начинается с пробела
        int64_t space = str.find(' ');

        // здесь можно избавиться от проверки на равенство npos
        // если space == npos, метод substr ограничит возвращаемый string_view концом строки
        result.push_back(str.substr(0, space));
        str.remove_prefix(min(str.find_first_not_of(" ", space), str.size()));
    }

    return result;
}

vector<string_view> SplitIntoWordsView1(string_view str) {
    vector<string_view> result;
    const size_t pos_end = str.npos;


    while (!str.empty()) {

    auto pos = str.find_first_not_of(" ");
    if (pos == pos_end) {
        return result;
    }
    str.remove_prefix(pos);
        
        size_t space = str.find(' ');
        if (space == pos_end){
            result.push_back(str);
            break;
        }
        //cout << " " << str.substr(0, space) << endl;
        result.push_back(str.substr(0, space));
        str.remove_prefix(space);

    }

    return result;
}

int main() {
    assert((SplitIntoWordsView("") == vector<string_view>{}));
    cout << "1" << endl;
    
    assert((SplitIntoWordsView("     ") == vector<string_view>{}));
    cout << "2" << endl;

    assert((SplitIntoWordsView("aaaaaaa") == vector{"aaaaaaa"sv}));
    cout << "3" << endl;
        
    assert((SplitIntoWordsView("a") == vector{"a"sv}));
    cout << "4" << endl;

    assert((SplitIntoWordsView("  a  ") == vector{"a"sv}));
    cout << "5" << endl;
    //for ( auto sv: SplitIntoWordsView("  a  ")){
    //    cout << sv << endl;
    //}
    assert((SplitIntoWordsView("a b c") == vector{"a"sv, "b"sv, "c"sv}));


    ///*
    assert((SplitIntoWordsView("a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    //*/
    cout << "All OK" << endl;
}