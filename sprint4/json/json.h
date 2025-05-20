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
    explicit Node(std::nullptr_t) : Node(){}
    explicit Node(Array array);
    explicit Node(Dict map);
    explicit Node(int value);
    explicit Node(std::string value);

    const Array& AsArray() const;
    const Dict& AsMap() const;
    int AsInt() const;
    int AsBool() const;
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

    void PrintNode(const Node& node, std::ostream& out) const {
        std::visit(
            [&](const auto& value){ PrintValue(value, out); },
            node.GetValue());
    }

private:
    
    Value data_;

    //inline const Color NoneColor{};

    // Шаблон, подходящий для вывода double и int
    /*
    template <typename Value>
    void PrintValue(const Value& value, std::ostream& out) {
        out << value;
    }
    */
    void PrintValue(const int& value, std::ostream& out) const {
        out << value;
    }
    void PrintValue(const double& value, std::ostream& out) const{
        out << value;
    }
    void PrintValue(const std::string& value, std::ostream& out) const{
        out << value;
    }
    void PrintValue(const Dict& value, std::ostream& out) const {
        for (const auto& e: value){
            out << e.first << " ";
        }
    }
    void PrintValue(const Array& value, std::ostream& out) const{
        for ([[maybe_unused]]const auto& _: value){
            out << " ";
        }
    }
    // Перегрузка функции PrintValue для вывода значений null
    void PrintValue(std::nullptr_t, std::ostream& out) const {
        out << "null";
    }
    // Другие перегрузки функции PrintValue пишутся аналогично
    struct OstreamSolutionPrinter {
        std::ostream& out;
        void operator()([[maybe_unused]] std::nullptr_t nptr) const {
            out << "none";
        }
        void operator()(int data) const {
            out << data;
        }
        void operator()(double data) const {
            out << data;
        }
        void operator()(std::string data) const {
            out << data;
        }
        void operator()(Dict data) const {
            for (const auto& e: data){
                out << e.first << " ";
            }
        }
        void operator()([[maybe_unused]] Array data) const {
            out << " ";
        }
    };
    /*
    void PrintNode(const Node& node, std::ostream& out) {
        std::visit(OstreamSolutionPrinter{out}, node.data_);
    }
    */

};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json