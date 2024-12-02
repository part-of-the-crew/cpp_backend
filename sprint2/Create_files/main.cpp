#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void CreateFiles(const string& config_file) {
    ifstream input_file(config_file);
    ofstream output_file;
    string line;

    while (getline(input_file, line)) {
        if (line[0] != '>') {
            output_file.close();
            output_file.open(line);
            continue;
        }

        output_file << line.substr(1) << endl;
    }
}

string GetLine(istream& in) {
    string s;
    getline(in, s);
    return s;
}

int main() {
    ofstream("test_config.txt"s) << "a.txt\n"
                                    ">10\n"
                                    ">abc\n"
                                    "b.txt\n"
                                    ">123"s;

    CreateFiles("test_config.txt"s);
    ifstream in_a("a.txt"s);
    assert(GetLine(in_a) == "10"s && GetLine(in_a) == "abc"s);

    ifstream in_b("b.txt"s);
    assert(GetLine(in_b) == "123"s);
}
/*

void CreateFiles(const string& config_file) {
    ifstream config;
    ofstream output;
    config.open(config_file);
    string line;
    string buffer;
    while (getline(config, line)) {
        if (line[0] == '>') {
            output << line.erase(0, 1) << endl;
        }else{
            if (output.is_open()) {
                output.flush();
                output.close();
            }
            output.open(line);
        }
    }
    output.close();
    config.close();
}
*/