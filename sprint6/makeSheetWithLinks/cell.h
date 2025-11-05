#pragma once

#include "common.h"
#include "formula.h"


class Cell : public CellInterface {
public:
    Cell();
    ~Cell();
    Cell(Cell&&) noexcept = default;
    Cell& operator=(Cell&&) noexcept = default;

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    
    std::unique_ptr<Impl> impl_;
};

std::ostream& operator<<(std::ostream& out, const CellInterface::Value& value);