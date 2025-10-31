#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <utility>


// -----------------------
// EmptyImpl
// -----------------------
std::string Cell::EmptyImpl::GetText() const {
    return {};
}

Cell::Value Cell::EmptyImpl::GetValue() const {
    return std::string{};
}

// -----------------------
// TextImpl
// -----------------------
Cell::TextImpl::TextImpl(std::string text)
    : text_(std::move(text)) {}

std::string Cell::TextImpl::GetText() const {
    return text_;
}

Cell::Value Cell::TextImpl::GetValue() const {
    if (!text_.empty() && text_.front() == ESCAPE_SIGN)
        return text_.substr(1);
    return text_;
}

// -----------------------
// FormulaImpl
// -----------------------
Cell::FormulaImpl::FormulaImpl(std::string expression)
    : formula_(ParseFormula(expression.substr(1))) {}

std::string Cell::FormulaImpl::GetText() const {
    return "=" + formula_->GetExpression();
}

Cell::Value Cell::FormulaImpl::GetValue() const {
    try {
        auto value = formula_->Evaluate();
        if (std::holds_alternative<FormulaError>(value))
            return std::get<FormulaError>(value);
        return std::get<double>(value);
    } catch (const FormulaException& e) {
        return FormulaError(e.what());
    }
}

// ====================== Cell Implementation ======================

Cell::Cell(){}

Cell::~Cell() {}

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


