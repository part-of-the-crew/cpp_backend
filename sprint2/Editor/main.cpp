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

// Напишите код упрощенного текстового редактора здесь
class Editor {
public:
    Editor() = default;
    // сдвинуть курсор влево
    void Left() {
        if (cursor_pos > 0) --cursor_pos;
    }
    // сдвинуть курсор вправо 
    void Right(){
        if (text.size() > cursor_pos) ++cursor_pos;
    }
    // вставить символ token
    void Insert(char token) {
        text.insert(text.begin() + cursor_pos, token);
        ++cursor_pos;
    }
    // вырезать не более tokens символов, начиная с текущей позиции курсора
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

    // cкопировать не более tokens символов, начиная с текущей позиции курсора
    void Copy(size_t tokens = 1) {
        clipboard.clear();
        for (size_t i = 0; i < tokens && (cursor_pos + i) < text.size(); ++i) {
            clipboard.push_back(text[cursor_pos + i]);
            //++cursor_pos;
        }
    }
    // вставить содержимое буфера в текущую позицию курсора
    void Paste() {
        text.insert(text.begin() + cursor_pos, clipboard.begin(), clipboard.end());
        cursor_pos += clipboard.size();
    }

    // получить текущее содержимое текстового редактора
    std::string GetText() const{
        return std::string(text.begin(), text.end());
    }
        size_t cursor_pos = 0;
private:
    std::deque<char> text;

    std::deque<char> clipboard;  // буфер обмена для копирования и вставки символов

};

int main() {
    Editor editor;
    const std::string text = "hello, world"s;
    for (char c : text) {
        editor.Insert(c);
    }
    // Текущее состояние редактора: `hello, world|`
    assert(editor.GetText() == "hello, world");
    assert(editor.cursor_pos == 12);
    for (size_t i = 0; i < text.size(); ++i) {
        editor.Left();
    }
    assert(editor.GetText() == "hello, world");
    assert(editor.cursor_pos == 0);
    // Текущее состояние редактора: `|hello, world`
    editor.Cut(7);
    // Текущее состояние редактора: `|world`
    //editor.GetText();
    assert(editor.GetText() == "world");
    assert(editor.cursor_pos == 0);
    // в буфере обмена находится текст `hello, `
    for (size_t i = 0; i < 5; ++i) {
        editor.Right();
    }
    assert(editor.GetText() == "world");
    assert(editor.cursor_pos == 5);
    // Текущее состояние редактора: `world|`
    editor.Insert(',');
    editor.Insert(' ');
    assert(editor.GetText() == "world, ");
    assert(editor.cursor_pos == 7);
    // Текущее состояние редактора: `world, |`
    editor.Paste();
    //editor.GetText();
    assert(editor.GetText() == "world, hello, ");
    assert(editor.cursor_pos == 14);
    // Текущее состояние редактора: `world, hello, |`
    editor.Left();
    editor.Left();
    assert(editor.GetText() == "world, hello, ");
    assert(editor.cursor_pos == 12);
    //Текущее состояние редактора: `world, hello|, `
    editor.Cut(3);  // Будут вырезаны 2 символа
    //assert(editor.GetText() == "world, hello");
    assert(editor.cursor_pos == 12);
    // Текущее состояние редактора: `world, hello|`
    std::cout << editor.GetText() << std::flush;
    return 0;
}
