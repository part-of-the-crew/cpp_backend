#include "json.h"

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;
    //std::string s;
    //getline(input, s);
    char c = 0;
    for (; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    if (c != ']')
        throw ParsingError("Failed to vector from stream"s);

    return Node(move(result));
}

Node LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return Node(std::stoi(parsed_num));
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return Node(std::stod(parsed_num));
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

// Считывает содержимое строкового литерала JSON-документа
// Функцию следует использовать после считывания открывающего символа ":
Node LoadString(std::istream& input) {
    using namespace std::literals;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    char ch{};

    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }
    return Node(std::move(s));
}

Node LoadDict(istream& input) {
    Dict result;
    char c{};
    for (; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }
    if (c != '}')
        throw ParsingError("Failed to map from stream"s);

    return Node(move(result));
}
Node ParseWord(istream& input) {
    std::string s;
    char c = 0;
    for (;input >> c;) {
        s.push_back(c);

        if (s == "null"){
            if (!(input >> c) || c == '\t' || c == '\r' || c == '\n' || c == ' ' ||
                c == ',' || c == '}' || c == ']'){
                input.putback(c);
                return Node{};
            }
            else
                throw ParsingError("Wrong word null");
        }
        if (s == "true"){
            if (!(input >> c) || c == '\t' || c == '\r' || c == '\n' || c == ' ' || 
                c == ',' || c == '}' || c == ']'){
                input.putback(c);
                return Node{true};
            }
            else
                throw ParsingError("Wrong word true");

        }
        if (s == "false"){
            if (!(input >> c) || c == '\t' || c == '\r' || c == '\n' || c == ' ' || 
                c == ',' || c == '}' || c == ']'){
                    input.putback(c);
                    return Node{false};
                }
            else
                throw ParsingError("Wrong word false " + s + c);
        }
        if (s.size() == 4 && s != "fals")
            throw ParsingError("Wrong word 4");
        
        if (s.size() == 5)
            throw ParsingError("Wrong word 5");
    }
    std::string ex;
    getline (input, ex);
    throw ParsingError("Wrong word: " + s + ex);
}
Node LoadNode(istream& input) {
    char c;
    do {
        input >> c;
    } while (c == '\t' || c == '\r' || c == '\n' || c == ' ');
    
    try {
        if (c == '[') {
            return LoadArray(input);
        } else if (c == '{') {
            return LoadDict(input);
        } else if (c == '"') {
            return LoadString(input);
        } else if (c == 'n' || c == 't' || c == 'f') {
            input.putback(c);
            return ParseWord(input);
        } else {
            input.putback(c);
            return LoadNumber(input);
        }
    } catch (const std::exception& e) {
        throw;  // Re-throws the current exception
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

Node::Node(double value)
    : data_(value) {
}

Node::Node(string value)
    : data_(move(value)) {
}

Node::Node(bool value)
    : data_(value) {
}

const Array& Node::AsArray() const {
    if (Node::IsArray())
        return std::get<Array>(data_);
    else
        throw std::logic_error {"my exception1"};
}

const Dict& Node::AsMap() const {
    if (Node::IsMap())
        return std::get<Dict>(data_);
    else
        throw std::logic_error {"my exception2"};
}

int Node::AsInt() const {
    if (Node::IsInt())
        return std::get<int>(data_);
    else
        throw std::logic_error {"my exception3"};
}
int Node::AsBool() const {
    if (Node::IsBool())
        return std::get<bool>(data_);
    else
        throw std::logic_error {"my exception4"};
}
double Node::AsDouble() const {
    if (Node::IsPureDouble())
        return std::get<double>(data_);
    else if (Node::IsInt())
        return static_cast<double> (std::get<int>(data_));
    else
        throw std::logic_error {"my exception5"};
}
const string &Node::AsString() const
{
    if (Node::IsString())
        return std::get<std::string>(data_);
    else
        throw std::logic_error {"my exception6"};
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

bool Node::IsBool() const
{
    return std::holds_alternative<bool>(data_);
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
    try {
        return Document{LoadNode(input)};
    } catch (const std::exception& e) {
        throw;  // Re-throws the current exception
    }
}    


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

void PrintNode(const Node& value, const PrintContext& ctx);

template <typename Value>
void PrintValue(const Value& value, const PrintContext& ctx) {
    ctx.out << value;
}

void PrintString(const std::string& value, std::ostream& out) {
    out.put('"');
    for (const char c : value) {
        switch (c) {
            case '\r':
                out << "\\r"sv;
                break;
            case '\n':
                out << "\\n"sv;
                break;
            case '\t':
                out << "\\t"sv;
                break;
            case '"':
                // Символы " и \ выводятся как \" или \\, соответственно
                [[fallthrough]];
            case '\\':
                out.put('\\');
                [[fallthrough]];
            default:
                out.put(c);
                break;
        }
    }
    out.put('"');
}

template <>
void PrintValue<std::string>(const std::string& value, const PrintContext& ctx) {
    PrintString(value, ctx.out);
}

template <>
void PrintValue<std::nullptr_t>(const std::nullptr_t&, const PrintContext& ctx) {
    ctx.out << "null"sv;
}

// В специализации шаблона PrintValue для типа bool параметр value передаётся
// по константной ссылке, как и в основном шаблоне.
// В качестве альтернативы можно использовать перегрузку:
// void PrintValue(bool value, const PrintContext& ctx);
template <>
void PrintValue<bool>(const bool& value, const PrintContext& ctx) {
    ctx.out << (value ? "true"sv : "false"sv);
}

template <>
void PrintValue<Array>(const Array& nodes, const PrintContext& ctx) {
    std::ostream& out = ctx.out;
    out << "[\n"sv;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const Node& node : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        inner_ctx.PrintIndent();
        PrintNode(node, inner_ctx);
    }
    out.put('\n');
    ctx.PrintIndent();
    out.put(']');
}

template <>
void PrintValue<Dict>(const Dict& nodes, const PrintContext& ctx) {
    std::ostream& out = ctx.out;
    out << "{\n"sv;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const auto& [key, node] : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        inner_ctx.PrintIndent();
        PrintString(key, ctx.out);
        out << ": "sv;
        PrintNode(node, inner_ctx);
    }
    out.put('\n');
    ctx.PrintIndent();
    out.put('}');
}


void PrintNode(const Node& node, const PrintContext& ctx) {
    std::visit(
        [&ctx](const auto& value) {
            PrintValue(value, ctx);
        },
        node.GetValue());
}

void Print(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(), PrintContext{output});
}

}  // namespace json