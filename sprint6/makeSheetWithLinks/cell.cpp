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

// ---------- Impl base ----------
class Cell::Impl {
public:
    virtual ~Impl();
    virtual std::string GetText() const = 0;
    virtual Value GetValue(const SheetInterface& sheet) = 0;
    virtual Value GetValue(const SheetInterface& sheet) const = 0;
    virtual std::vector<Position> GetReferencedCells() const = 0;
};

Cell::Impl::~Impl() = default;
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
    std::vector<Position> GetReferencedCells() const override {
        return {};
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
    std::vector<Position> GetReferencedCells() const override {
        return {};
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
        auto formula_evaluate = formula_->Evaluate(sheet);      
        if (std::holds_alternative<double>(formula_evaluate)) {
            return std::get<double>(formula_evaluate);
        } else {
            return std::get<FormulaError>(formula_evaluate);
        }
    }
    Value GetValue(const SheetInterface& sheet) override {
        
        auto formula_evaluate = formula_->Evaluate(sheet);   

        if (std::holds_alternative<double>(formula_evaluate)) {
            return std::get<double>(formula_evaluate);
        } else {
            return std::get<FormulaError>(formula_evaluate);
        }
    }
    std::vector<Position> GetReferencedCells() const override {
        return formula_->GetReferencedCells();
    }
private:
    std::unique_ptr<FormulaInterface> formula_;
};

// ====================== Cell Implementation ======================


Cell::Cell(Sheet &sheet)
    : sheet_(sheet)
    , impl_(std::make_unique<EmptyImpl>()) {}

Cell::~Cell() = default;

void Cell::SetImpl(std::string text) {
    if (text.empty()) {
        impl_ = std::make_unique<Cell::EmptyImpl>();
        return;
    }

    if (text.size() > 1 && text.front() == FORMULA_SIGN) {
        std::unique_ptr<FormulaImpl> temp_impl;

        try {
            temp_impl = std::make_unique<FormulaImpl>(text.substr(1));
        } catch (std::exception &e) {
            throw FormulaException("Formula error");
        }

        auto referenced_cells = temp_impl->GetReferencedCells();

        if (IsCircularDependencyDFS(referenced_cells)){
            throw CircularDependencyException{"Circular dependency"};
        }

        impl_ = std::move(temp_impl);
        return;
    }

    impl_ = std::make_unique<TextImpl>(std::move(text));

}
void Cell::Set(std::string text) {
    if (text == impl_->GetText()) {
        return;
    }
    SetImpl(std::move(text));

    // remove this from all parent->children_ sets
    for (auto parent : parents_)
        parent->children_.erase(this);

    parents_.clear();
    SetParents();
    cache_.reset();

    // Avoid mutating children_ while iterating it:
    std::vector<Cell*> children_copy(children_.begin(), children_.end());
    // Optionally clear our children_ before notifying them so they don't try to erase us
    children_.clear();
    for (auto child : children_copy) {
        if (child) child->cache_.reset();//child->Clear();
    }
}

CellInterface *Cell::CreateEmptyCell(const Position &pos) const {
  sheet_.SetCell(pos, "");
  return sheet_.GetCell(pos);
}

void Cell::SetParents() {
    for (const auto &parent_pos : GetReferencedCells()) {
        auto *parent = sheet_.GetCell(parent_pos);
        if (parent == nullptr) {
            parent = CreateEmptyCell(parent_pos);
        }

        static_cast<Cell*>(parent)->children_.insert(this);
        parents_.insert(static_cast<Cell *>(parent));
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();

    for (auto parent : parents_)
        parent->children_.erase(this);
    parents_.clear();

    cache_.reset();

    std::vector<Cell*> children_copy(children_.begin(), children_.end());
    children_.clear();
    for (auto child : children_copy) {
        if (child) 
            child->Clear();
    }
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue(sheet_);
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

bool Cell::IsReferenced() const {
    return !children_.empty();
}

std::unordered_set<Cell*> Cell::GetParents(){
    return parents_;
}
void Cell::AddToStack(std::stack<Position> &dest, const std::vector<Position> &src) {
    for (auto pos: src) {
        dest.push(pos);
    }
}

struct PositionHasher {
    size_t operator()(const Position& p) const noexcept {
        // 64-bit mix
        size_t h1 = std::hash<int>{}(p.row);
        size_t h2 = std::hash<int>{}(p.col);

        return h1 ^ (h2 << 15);
    }
};

bool Cell::IsCircularDependencyDFS(const std::vector<Position>& positions) {
    std::stack<Position> stack;
    AddToStack(stack, positions);
    std::unordered_map<Position, bool, PositionHasher> visited; 
    while (!stack.empty()) {
        Position current_pos = stack.top();
        stack.pop();
        bool check_pos = visited[current_pos];  
        if (check_pos) {
            continue;
        }
        visited[current_pos] = true;
        const CellInterface *current_cell = sheet_.GetCell(current_pos);  
        if (current_cell == this) {
            return true;
        }
        if (current_cell == nullptr) {
            current_cell = CreateEmptyCell(current_pos);
        }
        AddToStack(stack, current_cell->GetReferencedCells());
    }
    return false;
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
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
    using std::string_view_literals::operator""sv;
    
    switch (category_) {
        case Category::Ref:
            return "#REF!"sv;
        case Category::Value:
            return "#VALUE!"sv;
        case Category::Arithmetic:
            return "#ARITHM!"sv;
    }
    return "#ERROR!";  // fallback for unexpected cases
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}