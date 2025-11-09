#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <utility>
#include <variant>

#include <map>
#include <vector>
#include <stack>
#include <algorithm>

std::ostream& operator<<(std::ostream& out, const CellInterface::Value& value) {
    std::visit([&out](const auto& v) {
        out << v;
    }, value);
    return out;
}

// ---------- Impl base ----------
class Cell::Impl {
public:
    virtual ~Impl() = default;
    virtual std::string GetText() const = 0;
    virtual Value GetValue() const = 0;
};

// ---------- EmptyImpl ----------
class Cell::EmptyImpl : public Cell::Impl {
public:
    std::string GetText() const override {
        return "";
    }
    Value GetValue() const override {
        return std::string("");
    }
};

// ---------- TextImpl ----------
class Cell::TextImpl : public Cell::Impl {
public:
    explicit TextImpl(std::string text)
        : text_(std::move(text)) {}

    std::string GetText() const override {
        return text_;
    }

    Value GetValue() const override {
        if (!text_.empty() && text_.front() == ESCAPE_SIGN) {
            return text_.substr(1);  // skip escape char
        }
        return text_;
    }

private:
    std::string text_;
};

// ---------- FormulaImpl ----------
class Cell::FormulaImpl : public Cell::Impl {
public:
    explicit FormulaImpl(std::string expression)
        : formula_(ParseFormula(expression.substr(1))) {}

    std::string GetText() const override {
        return "=" + formula_->GetExpression();
    }

    Value GetValue(const SheetInterface& sheet) const override {
        FormulaInterface::Value result;
        try {
            result = formula_->Evaluate(sheet);
        } catch (const FormulaError& e) {
            return e;
        }

        if (std::holds_alternative<double>(result)) {
            return std::get<double>(result);
        } else {
            return std::get<FormulaError>(result);
        }
    }

private:
    std::unique_ptr<FormulaInterface> formula_;
};

// ====================== Cell Implementation ======================


Cell::Cell(Sheet &sheet, std::unordered_set<Cell*> deps)
    : sheet_(sheet)
    : updeps_(deps)
    , impl_(std::make_unique<EmptyImpl>()) {}

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    if (text.front() == FORMULA_SIGN && text.size() > 1){
        impl_ = std::make_unique<FormulaImpl>(text);
        return;
    }
    if (text.size() == 0){
        impl_ = std::make_unique<EmptyImpl>(EmptyImpl{});
        return;    
    }
    if (true || text.front() == ESCAPE_SIGN){
        impl_ = std::make_unique<TextImpl>(text);
        return;    
    }

}

void Cell::Clear() {
    impl_.reset();
}

Cell::Value Cell::GetValue(const SheetInterface& sheet) const {
    Value value;
    try {
        value = impl_->GetValue(sheet);
    } catch (const FormulaException& error ){
        throw error;
    }
    return value;
}
std::string Cell::GetText() const {
    return impl_->GetText();
}



bool IsCircularDependencyDFS() {
    
    enum class State { Unvisited, Visiting, Done };
    std::unordered_map<const Cell*, State> state;
    std::stack<std::pair<const Cell*, std::unordered_set<Cell*>::const_iterator>> stack;
    state[this] = State::Visiting;
    stack.push({this, downdeps_.begin()});
    while (!stack.empty()) {
        auto& [node, it] = stack.top();
        // finished exploring all dependencies
        if (it == node->GetDownstream().end()) {
            state[node] = State::Done;
            stack.pop();
            continue;
        }
        const Cell* next = *it;
        ++it; // move to next dependency
        if (state[next] == State::Unvisited) {
            state[next] = State::Visiting;
            stack.push({next, next->GetDownstream().begin()});
        } else if (state[next] == State::Visiting) {
            // back edge → cycle detected
            return true;
        }
    }
    return false;
}



bool IsCircularDependencyDFS1() {
    //int start_vertex, const std::map <int, std::vector<int>> &gr
    std::unordered_set <Cell*> proccesed;
    std::stack<Cell*> st;

    stack.push(start_vertex);  // Добавляем стартовую вершину в стек.
    while (!stack.empty()) {   // Пока стек не пуст:
        // Получаем из стека очередную вершину.
        // Это может быть как новая вершина, так и уже посещённая однажды.
        auto v = stack.top();
        stack.pop();
    
        if (!proccesed.contains(v)) {
            // Красим вершину в серый. И сразу кладём её обратно в стек:
            // это позволит алгоритму позднее вспомнить обратный путь по графу.
            proccesed.insert(v);

            stack.push(v);
            std::cout << v << " ";
            //b is Теперь добавляем в стек все непосещённые соседние вершины,
            // вместо вызова рекурсии
            auto it = gr.find(v);
            if (it != gr.end())  // Если в графе есть ребра из v.
              for (int w : it->second) {
                //std::cout << it->first << "x";
               //print (it->second);
                  // Для каждого исходящего ребра (v, w):
                  //std::cout << w << color[w] << "x";
                  if (color[w] == "white") {
                      stack.push(w);
                  }
              }
        } else if (color[v] == "gray") {
            // Серую вершину мы могли получить из стека только на обратном пути.
            // Следовательно, её следует перекрасить в чёрный.
            //color[v] = "black";
            return true;
        }
    }
    return false;
}