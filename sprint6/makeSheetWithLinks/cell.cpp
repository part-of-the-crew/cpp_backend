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
        /*
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
        return std::get<FormulaError>(result);
        */

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
            //auto e =  std::get<FormulaError>(formula_evaluate);
            //return std::string(e.ToString());(

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
        //CheckForCircularDependencies(referenced_cells);
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

/*
void Cell::Set(std::string text) {
    if (text == impl_->GetText()) {
        return;
    }   
    SetImpl(std::move(text));

    for (auto parent : parents_)
        parent->children_.erase(this);

    parents_.clear();
    SetParents();
    cache_.reset();

    for (auto child : children_)
        child->Clear();
}
/*
void Cell::Set(std::string text) {
    /*
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
    /*
    if (!text.empty() && text[0] == '=' && text.size() > 1) {
        impl_ = std::make_unique<FormulaImpl>(sheet_);
    } else {
        impl_ = std::make_unique<TextImpl>(sheet_);
    }
    DeleteDependences();
    AddDependences();
    impl_->Set(std::move(text));
    
    return;
}
*/
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
/*
void Cell::Clear() {
  impl_ = std::make_unique<EmptyImpl>();
  for (auto parent : parents_)
    parent->children_.erase(this);
  parents_.clear();
  //impl_->ClearCache();
  cache_.reset();
  for (auto child : children_)
    child->Clear();
}
*/
void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();

    // remove this from all parents' children_ sets
    for (auto parent : parents_)
        parent->children_.erase(this);
    parents_.clear();

    cache_.reset();

    // Copy children pointers to avoid iterator invalidation when child->Clear() mutates parents/children
    std::vector<Cell*> children_copy(children_.begin(), children_.end());
    children_.clear(); // remove all children before telling them to clear
    for (auto child : children_copy) {
        if (child) child->Clear();
    }
}

Cell::Value Cell::GetValue() const {
    //auto e = impl_->GetValue(sheet_);
    //return std::to_string(e.index());
    //            return FormulaError(FormulaError::Category::Value);
    return impl_->GetValue(sheet_);
}
/*
Cell::Value Cell::GetValue() {
    return impl_->GetValue(sheet_);
}
*/
std::string Cell::GetText() const {
    return impl_->GetText();
}

bool Cell::IsReferenced() const {
  return !children_.empty();
}

std::unordered_set<Cell*> Cell::GetParents(){
    return parents_;
}
void Cell::AddToStack(std::stack<Position> &destination, const std::vector<Position> &source) {
  for (Position pos : source) {
    destination.push(pos);
  }
}

std::stack<Position> Cell::CreateStack(const std::vector<Position> &referenced_cells) {
  std::stack<Position> stack_positions;
  AddToStack(stack_positions, referenced_cells);

  return stack_positions;
}
bool Cell::IsCircularDependencyDFS(const std::vector<Position>& positions) {
  std::stack<Position> stack_positions = CreateStack(positions);
  std::map<Position, bool> visited_cells;

  while (!stack_positions.empty()) {
    Position current_pos = stack_positions.top();
    stack_positions.pop();
    bool check_pos = visited_cells[current_pos];

    if (check_pos) {
      continue;
    }

    visited_cells[current_pos] = true;
    const CellInterface *current_cell = sheet_.GetCell(current_pos);

    if (current_cell == this) {
      return true;//throw CircularDependencyException{"Circular dependency"};
    }

    if (current_cell == nullptr) {
      current_cell = CreateEmptyCell(current_pos);
    }

    AddToStack(stack_positions, current_cell->GetReferencedCells());
  }
  return false;
}
/*
bool Cell::IsCircularDependencyDFS(const std::vector<Position>& positions) {
    std::unordered_set<CellInterface*> downdeps;
    for (auto pos: positions){
        auto pCell = sheet_.GetCell(pos);
        downdeps.insert(pCell);
    }

    enum class State { Unvisited, Visiting, Done };
    std::unordered_map<const CellInterface*, State> state;
    std::stack<std::pair<const CellInterface*, std::unordered_set<CellInterface*>::const_iterator>> stack;
    state[this] = State::Visiting;
    stack.push({this, downdeps.begin()});
    while (!stack.empty()) {
        auto& [node, it] = stack.top();
        // finished exploring all dependencies
        if (it == node->GetChildren().end()) {
            state[node] = State::Done;
            stack.pop();
            continue;
        }
        const CellInterface* next = *it;
        ++it; // move to next dependency
        if (state[next] == State::Unvisited) {
            state[next] = State::Visiting;
            stack.push({next, next->GetChildren().begin()});
        } else if (state[next] == State::Visiting) {
            // back edge → cycle detected
            return true;
        }
    }
    return false;
}
*/
/*
std::unordered_set<Cell*> Cell::GetDownstream(){
    return downdeps_;
}

std::unordered_set<Cell*> Cell::GetDownstream() const {
    return downdeps_;
}
*/
std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}
/*
    //Set dependencies for cache invalidation
void Cell::SetDependencies(std::unordered_set<Position> dep){
    updeps_ = dep;
    
}
    //Get dependencies for cache invalidation
std::unordered_set<Cell*> Cell::GetDependencies() {
    return updeps_;
}
*/
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
            return "#DIV/0!"sv;
    }
    return "#ERROR!";  // fallback for unexpected cases
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}