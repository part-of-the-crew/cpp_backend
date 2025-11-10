#include "cell.h"
#include "sheet.h"

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
#include <cctype>

std::ostream& operator<<(std::ostream& out, const CellInterface::Value& value) {
    std::visit([&out](const auto& v) {
        out << v;
    }, value);
    return out;
}
bool IsNumber(const std::string& s) {
    if (s.empty()) return false;
    char* end = nullptr;
    std::strtod(s.c_str(), &end);
    return end == s.c_str() + s.size(); // true if fully converted
}
// ---------- Impl base ----------
class Cell::Impl {
public:
    virtual ~Impl() = default;
    virtual std::string GetText() const = 0;
    virtual Value GetValue(const SheetInterface& sheet) = 0;
    virtual Value GetValue(const SheetInterface& sheet) const = 0;
};

// ---------- EmptyImpl ----------
class Cell::EmptyImpl : public Cell::Impl {
public:
    std::string GetText() const override {
        return "";
    }
    Value GetValue(const SheetInterface& sheet) const override {
        return std::string("");
    }
    Value GetValue(const SheetInterface& sheet) override {
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

    Value GetValue(const SheetInterface& sheet) const {
        if (!text_.empty() && text_.front() == ESCAPE_SIGN) {
            return text_.substr(1);  // skip escape char
        }
        return text_;
    }
    Value GetValue(const SheetInterface& sheet) {
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
        : formula_(ParseFormula(expression)) {}

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
        }
        if (std::holds_alternative<FormulaError>(result)) {
            return std::get<FormulaError>(result);
        }
    }
    Value GetValue(const SheetInterface& sheet) override {
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


Cell::Cell(Sheet &sheet)
    : sheet_(sheet)
    , impl_(std::make_unique<EmptyImpl>()) {}

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    if (IsNumber(text)) {
        impl_ = std::make_unique<FormulaImpl>(text);
        return;
    }
    if (text.front() == FORMULA_SIGN && text.size() > 1){
        impl_ = std::make_unique<FormulaImpl>(text.substr(1));
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

Cell::Value Cell::GetValue() const {
    Value value;
    try {
        value = impl_->GetValue(sheet_);
    } catch (const FormulaException& error ){
        throw error;
    }
    return value;
}

Cell::Value Cell::GetValue() {
    Value value;
    try {
        value = impl_->GetValue(sheet_);
    } catch (const FormulaException& error ){
        throw error;
    }
    return value;
}

std::string Cell::GetText() const {
    return impl_->GetText();
}



bool Cell::IsCircularDependencyDFS() {
    
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

std::unordered_set<Cell*> Cell::GetDownstream(){
    return downdeps_;
}

std::unordered_set<Cell*> Cell::GetDownstream() const {
    return downdeps_;
}

std::vector<Position> Cell::GetReferencedCells() const {
    std::vector<Position> vec;
    for (auto dep: downdeps_) {
        vec.push_back(sheet_.GetCellByPtr(dep));
    }
    return vec;
}

    //Set dependencies for cache invalidation
void Cell::SetDependencies(std::unordered_set<Cell*> dep){
    updeps_ = dep;
    
}
    //Get dependencies for cache invalidation
std::unordered_set<Cell*> Cell::GetDependencies() {
    return updeps_;
}

//----------------------------------------------------------
FormulaError::FormulaError(Category category)
    : category_(category) {}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
    switch (category_) {
        case Category::Ref:
            return "#REF!";
        case Category::Value:
            return "#VALUE!";
        case Category::Arithmetic:
            return "#DIV/0!";
    }
    return "#ERROR!";  // fallback for unexpected cases
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}