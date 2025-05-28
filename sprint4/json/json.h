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
        return data_ == other.data_;
    }

    bool operator!=(const Node& other) const {
        return !(*this == other);
    } 
    const Value& GetValue() const { return data_; }

private:
    
    Value data_;
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    bool operator==(const Document& other) const {
        return root_ == other.root_;
    }

    bool operator!=(const Document& other) const {
        return !(*this == other);
    }
private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);


}  // namespace json