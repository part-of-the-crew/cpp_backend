#pragma once

#include <unordered_set>

#include "common.h"
// #include "sheet.h"
#include "FormulaAST.h"
#include "formula.h"

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell() override;
    Cell(Cell&&) noexcept = default;
    Cell& operator=(Cell&&) noexcept = default;

    void Set(std::string text);
    void Clear();

    CellInterface::Value GetValue() const override;

    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;
    bool IsReferenced() const;
    std::unordered_set<Cell*> GetParents();

private:
    bool IsCircularDependencyDFS(const std::vector<Position>& positions);

    void SetImpl(std::string text);
    void AddToStack(std::stack<Position>& destination, const std::vector<Position>& source);

    std::stack<Position> CreateStack(const std::vector<Position>& referenced_cells);
    Sheet& sheet_;

    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    std::unordered_set<Cell*> parents_;   // for Circular
    std::unordered_set<Cell*> children_;  // for Cache
    std::unique_ptr<Impl> impl_;
    mutable std::optional<FormulaInterface::Value> cache_;

    CellInterface* CreateEmptyCell(const Position& pos) const;
    void SetParents();
};

std::ostream& operator<<(std::ostream& out, const CellInterface::Value& value);