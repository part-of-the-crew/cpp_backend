#pragma once

#include "cell.h"
#include "common.h"

#include <unordered_map>
#include <functional>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

	// Можете дополнить ваш класс нужными полями и методами

private:
	// Можете дополнить ваш класс нужными полями и методами
    Size printableSize_;
    void Print(std::ostream& output) const;
    void UpdatePrintableSize();
    /*
    // Custom hash for Cell
    struct CellHash {
        std::size_t operator()(const Cell& c) const noexcept {
        return std::hash<int>()(p.row) * 31 + std::hash<int>()(p.col);
    }
        */
    std::unordered_map<int, std::unordered_map<int, std::unique_ptr<Cell>>> sheet_;
};