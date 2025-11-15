#include "sheet.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

#include "common.h"

using namespace std::literals;

Sheet::~Sheet() = default;

void Sheet::SetCell(Position pos, std::string text) {
    IsValidPosition(pos);

    if (pos.col > printableSize_.cols - 1) {
        printableSize_.cols = pos.col + 1;
    }
    if (pos.row > printableSize_.rows - 1) {
        printableSize_.rows = pos.row + 1;
    }

    auto& row = sheet_[pos.row];
    if (row.find(pos.col) == row.end()) {
        row[pos.col] = std::make_unique<Cell>(*this);
    }
    row[pos.col]->Set(std::move(text));
}

const CellInterface* Sheet::GetCell(Position pos) const {
    IsValidPosition(pos);

    auto row_it = sheet_.find(pos.row);
    if (row_it == sheet_.end()) {
        return nullptr;
    }

    auto col_it = row_it->second.find(pos.col);
    if (col_it == row_it->second.end()) {
        return nullptr;
    }

    return col_it->second.get();
}

CellInterface* Sheet::GetCell(Position pos) {
    IsValidPosition(pos);

    auto row_it = sheet_.find(pos.row);
    if (row_it == sheet_.end()) {
        return nullptr;
    }

    auto col_it = row_it->second.find(pos.col);
    if (col_it == row_it->second.end()) {
        return nullptr;
    }

    return col_it->second.get();
}

void Sheet::UpdatePrintableSize() {
    if (sheet_.empty()) {
        printableSize_ = {0, 0};
        return;
    }

    int max_row = 0;
    int max_col = 0;

    for (const auto& [r, row_map] : sheet_) {
        if (row_map.empty())
            continue;
        max_row = std::max(max_row, r + 1);
        for (const auto& [c, _] : row_map) {
            max_col = std::max(max_col, c + 1);
        }
    }

    printableSize_ = {max_row, max_col};
}

void Sheet::ClearCell(Position pos) {
    IsValidPosition(pos);

    auto row_it = sheet_.find(pos.row);
    if (row_it == sheet_.end()) {
        return;  // Row not found
    }

    auto& row = row_it->second;
    auto cell_it = row.find(pos.col);
    if (cell_it == row.end()) {
        return;  // Cell not found
    }
    Cell* ptr = row_it->second.at(pos.col).get();
    cellPtr_.erase(ptr);  // remove from map
    // Erase the cell completely (not just clear content)
    row.erase(cell_it);

    if (row.empty()) {
        sheet_.erase(row_it);
    }

    // Recalculate printable area after removing
    UpdatePrintableSize();
}

Size Sheet::GetPrintableSize() const {
    return printableSize_;
}

void Sheet::PrintCells(std::ostream& output, const std::function<void(const CellInterface&)>& printCell) const {
    for (int r = 0; r < printableSize_.rows; ++r) {
        for (int c = 0; c < printableSize_.cols; ++c) {
            if (sheet_.find(r) != sheet_.end() && sheet_.at(r).find(c) != sheet_.at(r).end()) {
                printCell(*sheet_.at(r).at(c));
            }
            if (c < printableSize_.cols - 1) {
                output << "\t";
            }
        }
        output << "\n";
    }
}

void Sheet::PrintValues(std::ostream& output) const {
    PrintCells(output, [&output](const CellInterface& cell) { output << cell.GetValue(); });
}
void Sheet::PrintTexts(std::ostream& output) const {
    PrintCells(output, [&output](const CellInterface& cell) { output << cell.GetText(); });
}

void Sheet::IsValidPosition(Position pos) const {
    using std::string_literals::operator""s;

    if (!pos.IsValid()) {
        throw InvalidPositionException{"InvalidPosition"s};
    }
}
std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}