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

path FindPath(const path& in_file, const vector<path>& include_dir){
    for (const auto &path: include_dir) {
        //cerr << "Processing in include_dir" << path << "..." << in_file << endl;
        if (std::filesystem::exists(path / in_file)) {
            //std::cerr << "The file" << in_file << " exists. " << "in path " << path << "\n";
            return path;
        }
    }
    //std::cerr << "The file" << in_file << " not exists.\n";
    return {};
}

bool FindNext(const path& in_file, ofstream &out, const vector<path>& include_dir, path cur_path) {
    static const regex pattern1(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
    static const regex pattern2(R"/(\s*#\s*include\s*<([^>]*)>\s*)/");
    smatch match; // To store regex match results

    const static size_t min_sz {"#include\"\""s.length()};
    string line;
    int line_number = 0;
    ifstream in;

    in.open(cur_path / in_file, ios::in);
    if (!in.is_open()) {
        //cerr << "File " << in_file << " cannot be located in cur " << incl_path << endl;
        cur_path = FindPath(in_file, include_dir);
        if (cur_path.empty()) {
            //cerr << "File " << in_file.filename() << " not found in include_dir" << endl;
            return false;
        }
        in.open(cur_path / in_file, ios::in);
        if (!in.is_open()) {
            //cerr << "failed open " << incl_path << endl;
            return false;
        }
    }
    while (getline(in, line)) {
        line_number++;
            
        if (line.length() > min_sz && std::regex_match(line, match, pattern1)) {
            path include_file = match[1].str(); // Capture the file name
            //cerr << " capturing " << include_file << "in  " << line_number << endl;
            if (!FindNext(include_file, out, include_dir, cur_path / in_file.parent_path())){
                cout << "unknown include file "s << include_file.string() << " at file "s
                     << cur_path.string() << "/"s << in_file.string() << " at line "s << line_number << endl;
                return false;
            }
        } else if (line.length() > min_sz && std::regex_match(line, match, pattern2)) {
            path include_file = match[1].str(); // Capture the file name
            //cerr << " capturing " << include_file << " " << line_number << endl;
            if (!FindNext(include_file, out, include_dir, cur_path / in_file.parent_path())) {
                cout << "unknown include file "s << include_file.string() << " at file "s
                     << cur_path.string() << "/"s << in_file.string() << " at line "s << line_number << endl;
                return false;
            }
        } else {
            out << line << '\n'; // Copy other lines as-is
            //cerr << " processing " << line_number << endl;
        }
    }
    return true;
}

// напишите эту функцию
bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_dir){
    {
        ifstream in(in_file, ios::in);
        if (!in) {
            //std::cerr << "in_file not found" << std::endl;
            return false;
        }
    }

    std::ofstream out(out_file);
    if (!out) {
        //std::cerr << "Failed to open output file: " << out_file << "\n";
        return false;
    }

    return FindNext(in_file.filename(), out, include_dir, in_file.parent_path());
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