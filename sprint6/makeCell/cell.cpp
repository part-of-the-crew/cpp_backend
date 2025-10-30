#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

// Реализуйте следующие методы
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

/*
class Cell::Impl
{
public:
    virtual ~Impl() = default;
    virtual std::string GetText() const = 0;
    virtual void Set(const std::string& context) = 0;
};

class Cell::EmptyImpl : Cell::Impl
{
public:
    EmptyImpl() = default;

    std::string GetText() const override
    {
        return ""s;
    }
....
};

class Cell::TextImpl : Cell::Impl
{
public:
    TextImpl() = default;
    TextImpl(const std::string& str) : context_(str)
    {
    }
....
};

class Cell::FormulaImpl : Cell::Impl
{
public:
    FormulaImpl() = delete;
    FormulaImpl(const std::string& str) : ast_(ParseFormulaAST(str))
....
};
*/