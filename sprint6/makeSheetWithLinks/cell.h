#pragma once


#include <unordered_set>


#include "common.h"
//#include "sheet.h"
#include "formula.h"
#include "FormulaAST.h"

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet &sheet);
    ~Cell() override;
    Cell(Cell&&) noexcept = default;
    Cell& operator=(Cell&&) noexcept = default;

    void Set(std::string text);
    void Clear();

    CellInterface::Value GetValue() const override;
    //CellInterface::Value GetValue();
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;
    bool IsReferenced() const;
    std::unordered_set<Cell*> GetParents();
    //std::unordered_set<Cell*> GetDownstream() const;
private:
    bool IsCircularDependencyDFS(const std::vector<Position>& positions);
    void SetImpl(std::string text);
    //Set dependencies for cache invalidation
    //void SetDependencies(std::unordered_set<Cell*> dep);
    //Get dependencies for cache invalidation
    //std::unordered_set<Cell*> GetDependencies();   

    void AddToStack(std::stack<Position> &destination, const std::vector<Position> &source);

    std::stack<Position> CreateStack(const std::vector<Position> &referenced_cells);
    Sheet &sheet_;

    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    std::unordered_set<Cell*> parents_;   //for Cache
    std::unordered_set<Cell*> children_;  //for Circular
    std::unique_ptr<Impl> impl_;
    mutable std::optional<FormulaInterface::Value> cache_;

    CellInterface *CreateEmptyCell(const Position &pos) const;
    void SetParents();
};

std::ostream& operator<<(std::ostream& out, const CellInterface::Value& value);