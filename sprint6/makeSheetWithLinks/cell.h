#pragma once


#include <unordered_set>
#include "common.h"
#include "sheet.h"
#include "formula.h"
#include "FormulaAST.h"

class Cell : public CellInterface {
public:
    Cell(Sheet &sheet, std::unordered_set<Cell*> deps);
    ~Cell();
    Cell(Cell&&) noexcept = default;
    Cell& operator=(Cell&&) noexcept = default;

    void Set(std::string text);
    void Clear();

    CellInterface::Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells();
    std::unordered_set<Cell*> GetDownstream();
private:
    bool IsCircularDependencyDFS();

    //Set dependencies for cache invalidation
    void SetDependencies(std::unordered_set<Cell*> dep);
    //Get dependencies for cache invalidation
    std::unordered_set<Cell*> GetDependencies();   



    Sheet &sheet_;

    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    std::unordered_set<Cell*> updeps_;   //for Cache
    std::unordered_set<Cell*> downdeps_; //for Circular
    std::unique_ptr<Impl> impl_;
    mutable std::optional<FormulaInterface::Value> cache_;
};

std::ostream& operator<<(std::ostream& out, const CellInterface::Value& value);