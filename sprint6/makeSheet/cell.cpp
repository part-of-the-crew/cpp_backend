#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <utility>
#include <variant>

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

    Value GetValue() const override {
        FormulaInterface::Value result;
        try {
            result = formula_->Evaluate();
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


Cell::Cell() : impl_(std::make_unique<EmptyImpl>()) {}

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

Cell::Value Cell::GetValue() const {
    Value value;
    try {
        value = impl_->GetValue();
    } catch (const FormulaException& error ){
        throw error;
    }
    return value;
}
std::string Cell::GetText() const {
    return impl_->GetText();
}
