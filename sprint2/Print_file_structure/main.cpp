#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;
namespace fs = std::filesystem;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

void PrintFileOrFolder(filesystem::file_status status) {
    
    if (status.type() == filesystem::file_type::regular) {
        cout << "Путь указывает на файл"s << endl;
    } else if (status.type() == filesystem::file_type::directory) {
        cout << "Путь указывает на папку"s << endl;
    } else {
        cout << "Путь указывает другой объект"s << endl;
    }
}
void PrintFileOrFolder(filesystem::path p) {
    error_code err;
    auto status = filesystem::status(p, err);
    
    if (err) {
        return;
    }
    //cout << p.string();
    for (const auto& part : p) {
        std::cout << part.string() << std::endl;
    }
    PrintFileOrFolder(status);
}

void printTree(ostream& dst, const fs::path& directory, const std::string& prefix = "  ") {
    (void)dst;
    vector <std::filesystem::directory_entry> v;
    vector <std::filesystem::directory_entry> v1(directory.begin(), directory.end());
    try {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            std::cerr << "Invalid directory: " << directory << '\n';
            return;
        }
        for (const auto& entry : fs::directory_iterator(directory)) {
            v.push_back(entry);
        }
        
        sort(v.begin(), v.end(), std::greater<std::filesystem::directory_entry>());
        
        for (const auto& entry : v) {
            dst << prefix << entry.path().filename().string() << '\n';

            if (fs::is_directory(entry)) {
                printTree(dst, entry.path(), prefix + "  ");
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}

void PrintTree(ostream& dst, const path& p){
    dst << p.begin()->filename().string() << endl;
    printTree(dst, p);
}

/*
// напишите эту функцию
void PrintTree(ostream& dst, const path& p){
    vector <std::filesystem::directory_entry> v;
    for (const auto& dir_entry: filesystem::directory_iterator(p)) {
        v.push_back(dir_entry);
    }
    sort(v.begin(), v.end(), std::greater<std::filesystem::directory_entry>());
    for (const auto &e: v){
        PrintFileOrFolder(e);
    }
    dst << "hi";
    
}
*/
int main() {
    error_code err;
    filesystem::remove_all("test_dir", err);
    filesystem::create_directories("test_dir"_p / "a"_p, err);
    filesystem::create_directories("test_dir"_p / "b"_p, err);

    ofstream("test_dir"_p / "b"_p / "f1.txt"_p);
    ofstream("test_dir"_p / "b"_p / "f2.txt"_p);
    ofstream("test_dir"_p / "a"_p / "f3.txt"_p);

    ostringstream out;
    PrintTree(out, "test_dir"_p);
    
    assert(out.str() ==
        "test_dir\n"
        "  b\n"
        "    f2.txt\n"
        "    f1.txt\n"
        "  a\n"
        "    f3.txt\n"s
    );
    
}
/*

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>



using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

void PrintTree(ostream& dst, const path& p, const filesystem::file_status& status, int offset) {
    dst << string(offset, ' ') << p.filename().string() << endl;

    vector<pair<path, filesystem::file_status>> items;

    if (status.type() == filesystem::file_type::directory) {
        for (const auto& dir_entry : filesystem::directory_iterator(p)) {
            items.push_back({dir_entry.path(), dir_entry.status()});
        }
        sort(items.begin(), items.end(), [](const auto& l, const auto& r) {
            return l.first.filename().string() > r.first.filename().string();
        });
        for (const auto& [file, status] : items) {
            PrintTree(dst, file, status, offset + 2);
        }
    }
}

void PrintTree(ostream& dst, const path& p) {
    PrintTree(dst, p, filesystem::status(p), 0);
}

int main() {
    error_code err;
    filesystem::remove_all("test_dir", err);
    filesystem::create_directories("test_dir"_p / "a"_p, err);
    filesystem::create_directories("test_dir"_p / "b"_p, err);

    ofstream("test_dir"_p / "b"_p / "f1.txt"_p);
    ofstream("test_dir"_p / "b"_p / "f2.txt"_p);
    ofstream("test_dir"_p / "a"_p / "f3.txt"_p);

    ostringstream out;
    PrintTree(out, "test_dir"_p);
    assert(out.str() ==
           "test_dir\n"
           "  b\n"
           "    f2.txt\n"
           "    f1.txt\n"
           "  a\n"
           "    f3.txt\n"s
    );
}
@vansergh
vansergh commented Jul 16, 2024

void PrintDir(ostream& dst, const path& p, int offset) {
    vector<filesystem::directory_entry> dir_content;
    for (const filesystem::directory_entry& entry : filesystem::directory_iterator(p)) {
        if (entry.status().type() == filesystem::file_type::directory) {
            dir_content.push_back(entry);
        }
        else {
            dir_content.push_back(entry);
        }
    }

    if (!dir_content.empty()) {
        string offset_str;
        for (unsigned i = 0; i < offset; ++i) {
            offset_str += "  ";
        }        
        sort(dir_content.begin(), dir_content.end(),
            [](const auto& left, const auto& right) {
                return left > right;
            });
        
        for (const filesystem::directory_entry& entry : dir_content) {
            dst << offset_str << entry.path().filename().string() << '\n';
            if (entry.status().type() == filesystem::file_type::directory) {
                PrintDir(dst, entry.path(), offset + 1);
            }
        }
        
    }
}

// напишите эту функцию
void PrintTree(ostream& dst, const path& p) {
    error_code err;
    const filesystem::file_status status = filesystem::status(p, err);
    if (err) {
        return;
    }
    dst << p.filename().string() << '\n';
    if (status.type() == filesystem::file_type::directory) {
        PrintDir(dst, p, 1);
    }
}

@ScartByPrime
ScartByPrime commented Dec 1, 2024 •

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}


void PrintTree(ostream& dst, const path& p, int offset) {
        if (p.empty()) {
        return;
    }
    
    std::vector<filesystem::directory_entry> entries(filesystem::directory_iterator(p), filesystem::directory_iterator{});

    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        return a.path().filename().string() > b.path().filename().string();
    });

    for (const auto& dir_entry : entries) {
        if (dir_entry.is_directory()) {
            dst << string(offset + 2, ' ') << dir_entry.path().filename().string() << endl;
            PrintTree(dst, dir_entry.path(), offset + 2);
        } else {
            dst << string(offset + 2, ' ') << dir_entry.path().filename().string() << endl;
        }
    }
}

void PrintTree(ostream& dst, const path& p) {
    int offset = 0;
    dst << p.filename().string() << endl;
    PrintTree(dst, p, offset);
}


int main() {
    error_code err;
    filesystem::remove_all("test_dir", err);
    filesystem::create_directories("test_dir"_p / "a"_p, err);
    filesystem::create_directories("test_dir"_p / "b"_p, err);

    ofstream("test_dir"_p / "b"_p / "f1.txt"_p);
    ofstream("test_dir"_p / "b"_p / "f2.txt"_p);
    ofstream("test_dir"_p / "a"_p / "f3.txt"_p);

    ostringstream out;
    PrintTree(out, "test_dir"_p);
    assert(out.str() ==
        "test_dir\n"
        "  b\n"
        "    f2.txt\n"
        "    f1.txt\n"
        "  a\n"
        "    f3.txt\n"s
    );
}

@part-of-the-crew
Comment

*/