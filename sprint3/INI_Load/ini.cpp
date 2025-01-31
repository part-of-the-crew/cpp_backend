#include <string_view>

#include "ini.h"
using namespace std;

namespace ini {

Section& Document::AddSection(std::string name) {
    return sections_[move(name)];
}
const Section& Document::GetSection(const std::string& name) const {
    auto it = sections_.find(name);
    if (it == sections_.end()) {
        static const Section empty_section;
        return empty_section;
    }
    return it->second;
}
std::size_t Document::GetSectionCount() const {
    return sections_.size();
}

string LRstrip(string_view line) {
    while (!line.empty() && isspace(line.front())) {
        line.remove_prefix(1);
    }
    while (!line.empty() && isspace(line.back())) {
        line.remove_suffix(1);
    }
    return string{line};
}

string Lstrip(string_view line) {
    while (!line.empty() && isspace(line.front())) {
        line.remove_prefix(1);
    }
    return string{line};
}

string_view Rstrip(string_view line) {
    while (!line.empty() && isspace(line.back())) {
        line.remove_suffix(1);
    }
    return line;
}

string_view UnWrap(string_view value) {
    if (!value.empty() && value.front() == '[') {
        value.remove_prefix(1);
    }
    if (!value.empty() && value.back() == ']') {
        value.remove_suffix(1);
    }
    return value;
}
std::istream& operator>>(std::istream& input, Document& doc){
    string line;
    string token;
    std::size_t start_token;
    while(getline(input, line)){
        if(line.empty() || line[0] == '#')
            continue;
        
        start_token = line.find_first_of('[');
        if (start_token != std::string::npos){
            token = UnWrap(line.substr(start_token, line.find_first_of(']', start_token)));
            doc.AddSection(token).clear();
            continue;
        }
        auto equals = line.find('=');
        string key = LRstrip(line.substr(0, equals));
        string value = LRstrip(line.substr(equals + 1, string::npos));
        doc.AddSection(token).emplace(move(key), move(value));
    }
    return input;
}

Document Load(std::istream& input){
    Document doc;
    input >> doc;
    return doc;
}
}