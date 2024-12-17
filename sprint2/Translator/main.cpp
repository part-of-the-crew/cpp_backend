#include <unordered_map>
#include <cassert>
#include <string>
#include <string_view>
#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>
#include <map>
#include <string>


using namespace std;

class Translator {
public:
    void Add(string_view source, string_view target) {
        const string_view source_view = GetClonedView(source);
        const string_view target_view = GetClonedView(target);
        forward_dict[source_view] = target_view;
        backward_dict[target_view] = source_view;
    }
    string_view TranslateForward(string_view source) const {
        return Translate(forward_dict, source);
    }
    string_view TranslateBackward(string_view target) const {
        return Translate(backward_dict, target);
    }

private:
    string_view GetClonedView(string_view s) {
        for (const auto* map_ptr : {&forward_dict, &backward_dict}) {
            const auto it = map_ptr->find(s);
            if (it != map_ptr->end()) {
                return it->first;
            }
        }
        return data.emplace_back(s);
    }

    static string_view Translate(const map<string_view, string_view>& dict, string_view s) {
        if (const auto it = dict.find(s); it != dict.end()) {
            return it->second;
        } else {
            return {};
        }
   
 }

    map<string_view, string_view> forward_dict;
    map<string_view, string_view> backward_dict;
    deque<string> data;
};


class Translator2 {
public:
    void Add(string_view source, string_view target) {
        string source_copy(source);
        string target_copy(target);
        
        // Update forward map
        auto it_forward = forward.find(source_copy);
        if (it_forward != forward.end()) {
            backward.erase(it_forward->second); // Remove old backward mapping
        }
        
        // Update backward map
        auto it_backward = backward.find(target_copy);
        if (it_backward != backward.end()) {
            forward.erase(it_backward->second); // Remove old forward mapping
        }

        // Add new mappings
        forward[source_copy] = target_copy;
        backward[target_copy] = source_copy;
    }

    string_view TranslateForward(string_view source) const {
        auto it = forward.find(string(source));
        if (it != forward.end()) {
            return it->second;
        }
        return string_view{}; // Return empty view if not found
    }

    string_view TranslateBackward(string_view target) const {
        auto it = backward.find(string(target));
        if (it != backward.end()) {
            return it->second;
        }
        return string_view{}; // Return empty view if not found
    }

private:
    unordered_map<string, string> forward; // Source -> Target
    unordered_map<string, string> backward; // Target -> Source
};

class Translator1 {
public:
    void Add(string_view source, string_view target){
        string source_ (source);
        string target_ (target);

        auto it = find_if( v.begin(), v.end(), [&](const pair<string, string> p){return p.first == source_;});

        if (it != v.end()) {
            it->second = target_;
            return;
        }

        auto it1 = find_if( v.begin(), v.end(), [&](const pair<string, string> p){return p.second == target_;});

        if (it1 != v.end()) {
            it1->first = source_;
            return;
        }


        v.push_back ({source_, target_});
    }
    string_view TranslateForward(string_view source) const {
        //print map
        //for (auto const & s : mf) cout << "source: " << s.first << ", target: " << s.second << endl;
        string source_ (source);
        //find in string v
        auto it = find_if( v.begin(), v.end(), [&source_](const pair<string, string> p){return p.first == source_;});

        if (it == v.end()) {
            return "";
        }  // not found in forward map, return empty string
        return it->second;
        
    }
    string_view TranslateBackward(string_view target) const {
        string target_ (target);
        auto it = find_if( v.begin(), v.end(), [&target_](const pair<string, string> p){return p.second == target_;});

        if (it == v.end()) {
            return "";
        }  // not found in forward map, return empty string
        return it->first;
    }

private:
    //std::map<std::string, std::string> mf;
    //std::map<std::string, std::string> mb;
    std::vector<std::pair <std::string, std::string>> v;
};

void TestSimple() {
    Translator translator;
{
    translator.Add(string("okno"s), string("window"s));
    translator.Add(string("stol"s), string("table"s));
}
    assert(translator.TranslateForward("okno"s) == "window"s);
    assert(translator.TranslateBackward("table"s) == "stol"s);
    assert(translator.TranslateForward("table"s) == ""s);
{
    translator.Add(string("okno"s), string("window1"s));
}
    assert(translator.TranslateForward("okno"s) == "window1"s);
    assert(translator.TranslateBackward("window1"s) == "okno"s);
{
    translator.Add(string("stol1"s), string("table"s));
}
    assert(translator.TranslateForward("stol1"s) == "table"s);
    assert(translator.TranslateBackward("table"s) == "stol1"s);
}

int main() {
    TestSimple();
}