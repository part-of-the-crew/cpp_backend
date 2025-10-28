#include "common.h"

#include <cctype>
#include <sstream>
#include <cmath>
#include <iostream>
//“AB15” — (14, 27).
const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;

const Position Position::NONE = {-1, -1};

// Реализуйте методы:
bool Position::operator==(const Position rhs) const {
    if (row == rhs.row && col == rhs.col){
        return true;
    }
    return false;
}

bool Position::operator<(const Position rhs) const {
    if (row < rhs.row){
        return true;
    }
    return false;   
}




bool Position::IsValid() const {
    if (col < 0 || col >= MAX_COLS) {
        return false;
    }
    if (row < 0 || row >= MAX_ROWS) {
        return false;
    }
    return true;
}


bool CheckValidity(const std::string_view &str)  {
    std::size_t i = 0; 
    if (str.size() < 2)
        return false;

    for (;i < str.size(); i++){
        if (str[i] < 'A' || str[i] > 'Z'){
            if (i == 0){
                return false;
            }
            break;
        }
    }
    if (i == str.size())
        return false;
    if (str[i] < '1' || str[i] > '9'){
        return false;
    }

    for (;i < str.size(); i++){
        if (str[i] < '0' || str[i] > '9'){
            return false;
        }
    }
    return true;
}


char ToLetter (int index){
    return static_cast <char>(index) + 'A';
}
int FromLetter (char c){
    return static_cast <char>('A' - c);
}
int base26ToDecimal(const std::string_view& column) {
    int result = 0;
    for (char c : column) {
        // Convert letter to number: 'A' -> 1, 'B' -> 2, ..., 'Z' -> 26
        int value = c - 'A' + 1;
        result = result * 26 + value;
    }
    return result;
}



std::string Position::ToString() const {
    std::string result;
    if (!IsValid())
        return result;

    int curcol = col;
    while (curcol >= 0) {
        int remainder = curcol % 26;
        result.insert(result.begin(), ToLetter(remainder)); // prepend the letter
        curcol = curcol / 26 - 1; // shift left in base-26
    }

    result += std::to_string(row + 1);
    return result;
}

Position Position::FromString(std::string_view str) {
    Position pos;
    if (CheckValidity(str) == false )
        return Position{-1, -1};
    std::size_t i = 0; // size of letters

    for (;i < str.size(); i++){
        if (str[i] < 'A' || str[i] > 'Z'){
            break;
        }
    }

    if (i > 3)
        return Position{-1, -1};

    pos.col = base26ToDecimal(str.substr(0, i)) - 1;

    if ((str.size() - i) > 5)
        return Position{-1, -1};
    pos.row = std::stoi(std::string(str.substr(i, str.size()))) - 1;

    return pos;
}