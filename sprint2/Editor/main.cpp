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

using namespace std::literals;


class Editor {
public:
    Editor() = default;

    void Left() {
        if (cursor_pos > 0) --cursor_pos;
    }

    void Right(){
        if (text.size() > cursor_pos) ++cursor_pos;
    }

    void Insert(char token) {
        text.insert(text.begin() + cursor_pos, token);
        ++cursor_pos;
    }

    void Cut(size_t tokens = 1) {
        clipboard.clear();

        // Limit the number of characters to cut to the remaining text size
        size_t cut_count = std::min(tokens, text.size() - cursor_pos);

        // Copy the characters to clipboard
        for (size_t i = 0; i < cut_count; ++i) {
            clipboard.push_back(text[cursor_pos + i]);
        }

        // Erase the characters from the text
        text.erase(text.begin() + cursor_pos, text.begin() + cursor_pos + cut_count);
    }

    void Copy(size_t tokens = 1) {
        clipboard.clear();
        for (size_t i = 0; i < tokens && (cursor_pos + i) < text.size(); ++i) {
            clipboard.push_back(text[cursor_pos + i]);
        }
    }

    void Paste() {
        text.insert(text.begin() + cursor_pos, clipboard.begin(), clipboard.end());
        cursor_pos += clipboard.size();
    }


    std::string GetText() const{
        return std::string(text.begin(), text.end());
    }

    //get the cursor position
    size_t GetCursorPosition() const {
        return cursor_pos;
    }


private:
    std::deque<char> text;
    std::deque<char> clipboard;  // buffer
    size_t cursor_pos = 0;

};

int main() {
    Editor editor;
    const std::string text = "hello, world"s;
    for (char c : text) {
        editor.Insert(c);
    }
    // Текущее состояние редактора: `hello, world|`
    assert(editor.GetText() == "hello, world");
    assert(editor.GetCursorPosition() == 12);
    for (size_t i = 0; i < text.size(); ++i) {
        editor.Left();
    }
    assert(editor.GetText() == "hello, world");
    assert(editor.GetCursorPosition() == 0);
    // Текущее состояние редактора: `|hello, world`
    editor.Cut(7);
    // Текущее состояние редактора: `|world`
    //editor.GetText();
    assert(editor.GetText() == "world");
    assert(editor.GetCursorPosition() == 0);
    // в буфере обмена находится текст `hello, `
    for (size_t i = 0; i < 5; ++i) {
        editor.Right();
    }
    assert(editor.GetText() == "world");
    assert(editor.GetCursorPosition() == 5);
    // Текущее состояние редактора: `world|`
    editor.Insert(',');
    editor.Insert(' ');
    assert(editor.GetText() == "world, ");
    assert(editor.GetCursorPosition() == 7);
    // Текущее состояние редактора: `world, |`
    editor.Paste();
    //editor.GetText();
    assert(editor.GetText() == "world, hello, ");
    assert(editor.GetCursorPosition() == 14);
    // Текущее состояние редактора: `world, hello, |`
    editor.Left();
    editor.Left();
    assert(editor.GetText() == "world, hello, ");
    assert(editor.GetCursorPosition() == 12);
    //Текущее состояние редактора: `world, hello|, `
    editor.Cut(3);  // Будут вырезаны 2 символа
    //assert(editor.GetText() == "world, hello");
    assert(editor.GetCursorPosition() == 12);
    // Текущее состояние редактора: `world, hello|`
    std::cout << editor.GetText() << std::flush;
    return 0;
}
