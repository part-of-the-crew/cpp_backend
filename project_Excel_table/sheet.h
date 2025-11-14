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


private:
    Size printableSize_;
    void UpdatePrintableSize();
    void PrintCells(std::ostream& output,
                    const std::function<void(const CellInterface&)>& printCell) const;

    void IsValidPosition(Position pos) const;
    
    std::unordered_map<int, std::unordered_map<int, std::unique_ptr<Cell>>> sheet_;
    std::unordered_map<const Cell*, Position> cellPtr_;
};