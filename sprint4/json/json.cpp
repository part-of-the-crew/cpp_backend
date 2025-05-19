#include "json.h"

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    return Node(move(result));
}

Node LoadInt(istream& input) {
    int result = 0;
    while (isdigit(input.peek())) {
        result *= 10;
        result += input.get() - '0';
    }
    return Node(result);
}

Node LoadString(istream& input) {
    string line;
    getline(input, line, '"');
    return Node(move(line));
}

Node LoadDict(istream& input) {
    Dict result;

    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }

    return Node(move(result));
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else {
        input.putback(c);
        return LoadInt(input);
    }
}

}  // namespace

Node::Node(Array array)
    : data_(move(array)) {
}

Node::Node(Dict map)
    : data_(move(map)) {
}

Node::Node(int value)
    : data_(value) {
}

Node::Node(string value)
    : data_(move(value)) {
}

const Array& Node::AsArray() const {
    if (Node::IsArray())
        return std::get<Array>(data_);
    else
        throw std::logic_error {""};
}

const Dict& Node::AsMap() const {
    if (Node::IsMap())
        return std::get<Dict>(data_);
    else
        throw std::logic_error {""};
}

int Node::AsInt() const {
    if (Node::IsInt())
        return std::get<int>(data_);
    else
        throw std::logic_error {""};
}

const string& Node::AsString() const {
    if (Node::IsString())
        return std::get<std::string>(data_);
    else
        throw std::logic_error {""};
}

bool Node::IsNull() const
{
    return std::holds_alternative<std::nullptr_t>(data_);
}

bool Node::IsInt() const
{
    return std::holds_alternative<int>(data_);
}

bool Node::IsDouble() const
{
    return std::holds_alternative<int>(data_) || std::holds_alternative<double>(data_);
}

bool Node::IsPureDouble() const
{
    return std::holds_alternative<double>(data_);
}

bool Node::IsString() const
{
    return std::holds_alternative<std::string>(data_);
}

bool Node::IsArray() const
{
    return std::holds_alternative<Array>(data_);
}

bool Node::IsMap() const
{
    return std::holds_alternative<Dict>(data_);
}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    (void) &doc;
    (void) &output;

    // Реализуйте функцию самостоятельно
}

}  // namespace json