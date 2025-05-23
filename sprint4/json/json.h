#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
    using Value = std::variant<std::nullptr_t, bool, int, double, std::string, Array, Dict>;

    Node() = default;
    Node(std::nullptr_t) : Node(){}
    Node(Array array);
    Node(Dict map);
    Node(int value);
    Node(double value);
    Node(std::string value);
    Node(bool value);

    const Array& AsArray() const;
    const Dict& AsMap() const;
    int AsInt() const;
    int AsBool() const;
    double AsDouble() const;
    const std::string& AsString() const;

    bool IsNull() const;
    bool IsInt() const;
    bool IsBool() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsMap() const;

    bool operator==(const Node& other) const {
        return data_.index() == other.data_.index() && data_ == other.data_;
    }

    bool operator!=(const Node& other) const {
        return !(*this == other);
    } 
    const Value& GetValue() const { return data_; }

    void PrintNode(const Node& node, std::ostream& out) const {
        std::visit(
            [&](const auto& value){ PrintValue(value, out); },
            node.GetValue());
    }

private:
    
    Value data_;

    void PrintValue(const int& value, std::ostream& out) const {
        out << value;
    }
    void PrintValue(const double& value, std::ostream& out) const{
        out << value;
    }
    void PrintValue(const std::string& value, std::ostream& out) const {
        std::string str{"\""};
        for (size_t i = 0; i < value.size(); i++){
            switch (value[i]) {
            case '\\':
                if (i < value.size() - 1 && (value[i + 1] == '\r' ||
                                             value[i + 1] == '\t' || 
                                             value[i + 1] == '\n'))
                {
                    str += "\\\\";
                    str += value[i + 1];
                    i++;
                } 
                else
                    str += "\\\\";
                break;
            case '\"':
                str += "\\\"";
                break;
            case '\r':
                str += '\\';
                str += 'r';
                break;
            case '\n':
                str += '\\';
                str += 'n';
                break;
            case '\t':
                str += '\\';
                str += 't';
                break;
                str.back() = value[i];
                break;

            default:
                str.push_back(value[i]);
                break;
            }
        }
        str += "\"";

        out << str;
    }
    void PrintValue(const Dict& value, std::ostream& out) const {
        out << '{';
        bool first = 0;
        for (const auto& e: value){
            if (first)
                out << ',';
            PrintNode(e.first, out);
            out << ": ";
            PrintNode(e.second, out);
            first = 1;
        }
        out << '}';
    }
    void PrintValue(const Array& value, std::ostream& out) const {
        out << '[';
        bool first = 0;
        for (const auto& _: value){
            if (first)
                out << ',';
            PrintNode(_, out);
            first = 1;
        }
        out << ']';
    }
    // Перегрузка функции PrintValue для вывода значений null
    void PrintValue(std::nullptr_t, std::ostream& out) const {
        out << "null";
    }
    void PrintValue(const bool& value, std::ostream& out) const {
        if (value)
            out << "true";
        else
            out << "false";
    }
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    bool operator==(const Document& other) const {
        return root_ == other.root_ && root_ == other.root_;
    }

    bool operator!=(const Document& other) const {
        return !(*this == other);
    }
private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

// Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    // Возвращает новый контекст вывода с увеличенным смещением
    PrintContext Indented() const {
        return {out, indent_step, indent_step + indent};
    }
};


}  // namespace json