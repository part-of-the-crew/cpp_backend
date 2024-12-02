#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;
using namespace std::string_literals;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

void PrintFloatInfo(const std::string& text) {
    static regex num_reg("\\s*([+-]?[0-9]+)\\.([0-9]*)(e[+-]?[0-9]+)?\\s*");
    smatch m;

    if (!regex_match(text, m, num_reg)) {
        cout << text << " - не число с плавающей точкой"s << endl;
    } else {
        cout << text << " - число с целой частью "s << m[1]
                     << ", дробной частью "s << m[2] << endl;
    }
}

string trim(string_view text) {
    static const auto r = regex(R"(\s+)");
    return regex_replace(text.data(), r, "");
}


void MatchFirst(const std::string& text, ifstream& in, ofstream& out, const vector<path>& include_dir) {
    // \s*#\s*include\s*"([^"]*)"\s*
    //static const regex num_reg("\\s*#\\s*include\\s*\"([^\"]*)\"\\s*");
    static const regex pattern(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
    smatch m;
    (void)in;
    (void)out;
    (void)include_dir;
    auto begin = std::sregex_iterator(text.begin(), text.end(), pattern);
    auto end = std::sregex_iterator();

    
    for (std::sregex_iterator it = begin; it != end; ++it) {
        //FindNext(it->str());
        std::string result = std::regex_replace(text, pattern, "fruit");
    }
}

path FindInInclude(const path& in_file, const vector<path>& include_dir){
    for (const auto &path: include_dir) {
        cout << "Processing in include_dir" << path << "..." << in_file << endl;
        if (std::filesystem::exists(path / in_file)) {
            std::cout << "The file exists.\n";
            return path;
        } else {
            std::cout << "The file does not exist.\n";
            return {};
        }
    }
}

bool FindNext(const path& in_file, string& dst, const vector<path>& include_dir) {
    static const regex pattern1(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
    static const regex pattern2(R"/(\s*#\s*include\s*<([^>]*)>\s*)/");
    smatch match; // To store regex match results
    string line;
    int line_number = 0;
    ifstream in;
    in.open(in_file, ios::in);
    if (in.is_open()) {
        while (getline(in, line)) {
            line_number++;
            if (std::regex_match(line, match, pattern1)) { // Match #include "filename"
                path include_file = static_cast<path>(match[1]); // Capture the file name
                cout << " capturing " << include_file << " " << line_number << endl;
                FindNext(in_file.parent_path() / include_file, dst, include_dir);
                dst += '\n';
            } else if (std::regex_match(line, match, pattern2)) { // Match #include "filename"
                path include_file = static_cast<path>(match[1]); // Capture the file name
                cout << " capturing " << include_file << " " << line_number << endl;
                FindNext(in_file.parent_path() / include_file, dst, include_dir);
                dst += '\n';
            } else {
                dst += line + '\n'; // Copy other lines as-is
                cout << " processing " << line_number << endl;
            }
        }
    } else {
        cout << "failed open " << in_file << endl;
        return false;
    }
    return true;
}

// напишите эту функцию
bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_dir){
    {
        ifstream in(in_file, ios::in);
        if (!in) {
            std::cerr << "in_file not found" << std::endl;
            return false;
        }
    }
    string temp;
    bool result = FindNext(in_file, temp, include_dir);

    ofstream out(out_file, ios::out);
    out << temp;

    return result;
}

string GetFileContents(string file) {
    ifstream stream(file);

    // конструируем string по двум итераторам
    return {(istreambuf_iterator<char>(stream)), istreambuf_iterator<char>()};
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
                "#include \"dir1/b.h\"\n"
                "// text between b.h and c.h\n"
                "#include \"dir1/d.h\"\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"
                "#   include<dummy.txt>\n"
                "}\n"s;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
                "#include \"subdir/c.h\"\n"
                "// text from b.h after include"s;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
                "#include <std1.h>\n"
                "// text from c.h after include\n"s;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
                "#include \"lib/std2.h\"\n"
                "// text from d.h after include\n"s;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"s;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"s;
    }

    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
                                  {"sources"_p / "include1"_p,"sources"_p / "include2"_p})));

    ostringstream test_out;
    test_out << "// this comment before include\n"
                "// text from b.h before include\n"
                "// text from c.h before include\n"
                "// std1\n"
                "// text from c.h after include\n"
                "// text from b.h after include\n"
                "// text between b.h and c.h\n"
                "// text from d.h before include\n"
                "// std2\n"
                "// text from d.h after include\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}