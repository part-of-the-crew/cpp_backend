#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

using namespace std;

template <typename Hash>
int FindCollisions(const Hash& hasher, istream& text) {
    int collisions = 0;
    string word;
    unordered_set<size_t> hash_set;
    unordered_set<string> str_set;
    while (text >> word) {
        if (!str_set.contains(word)) {
            size_t hash_value = hasher(word);
            str_set.insert(move(word));
            if (hash_set.contains(hash_value)){
                collisions++;
            } else {
                hash_set.insert(hash_value);
            }
        }
    }
    return collisions;
}
// Это плохой хешер. Его можно использовать для тестирования.
// Подумайте, в чём его недостаток
struct HasherDummy {
    size_t operator() (const string& str) const {
        size_t res = 0;
        for (char c : str) {
            res += static_cast<size_t>(c);
        }
        return res;
    }
};
struct DummyHash {
    size_t operator()(const string&) const {
        return 42;
    }
};

int main() {
    DummyHash dummy_hash;
    hash<string> good_hash;

    {
        istringstream stream("I love C++"s);
        cout << FindCollisions(dummy_hash, stream) << endl;
    }
    {
        istringstream stream("I love C++"s);
        cout << FindCollisions(good_hash, stream) << endl;
    }

    {
        istringstream stream("I I I"s);
        cout << FindCollisions(dummy_hash, stream) << endl;
    }

    {
        istringstream stream("A A B B A C"s);
        cout << FindCollisions(dummy_hash, stream) << endl;
    }
} 