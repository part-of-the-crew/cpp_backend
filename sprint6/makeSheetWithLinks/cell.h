#pragma once


#include <unordered_set>
#include "common.h"
#include "sheet.h"
#include "formula.h"
#include "FormulaAST.h"

class Cell : public CellInterface {
public:
    Cell(Sheet &sheet);
    ~Cell();
    Cell(Cell&&) noexcept = default;
    Cell& operator=(Cell&&) noexcept = default;

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells();

private:
    bool FindCircularDependency();
    bool SetDependencyDown(); //Set down dependencies for cash invalidation
    Sheet &sheet_;

    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    class CellImpl;
    std::unordered_set<Cell*> updeps_;   //for Cashe
    std::unordered_set<Cell*> downdeps_; //for Circular
    std::unique_ptr<Impl> impl_;
    mutable std::optional<FormulaInterface::Value> cache_;
};

std::ostream& operator<<(std::ostream& out, const CellInterface::Value& value);