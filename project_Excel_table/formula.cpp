#include "formula.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <sstream>

#include "FormulaAST.h"

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, const FormulaError& fe) {
    return output << "#ARITHM!";
}

std::optional<double> GetNumber(const std::string& s) {
    if (s.empty())
        return std::nullopt;

    char* end = nullptr;
    double res = std::strtod(s.c_str(), &end);

    // Check if the whole string was parsed as a number
    if (end == (s.c_str() + s.size()))
        return res;
    else
        return std::nullopt;
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(const std::string& expression) : ast_{ParseFormulaAST(expression)} {}

    Value Evaluate(const SheetInterface& sheet) const override {
        auto lambda = [&sheet](Position pos) -> double {
            auto cell = sheet.GetCell(pos);
            auto value = cell ? cell->GetValue() : 0.0;
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            }
            if (std::holds_alternative<std::string>(value)) {
                std::string str_value = std::get<std::string>(value);
                if (str_value.empty()) {
                    return 0;
                }
                auto res = GetNumber(str_value);
                if (res.has_value()) {
                    return *res;
                }
                throw FormulaError(FormulaError::Category::Value);
            }
            throw std::get<FormulaError>(value);
        };
        try {
            return ast_.Execute(lambda);
        } catch (FormulaError& fe) {
            return fe;
        }
    }
    std::string GetExpression() const override {
        std::ostringstream out;  // string-based output stream
        ast_.PrintFormula(out);  // print the AST into it
        return out.str();        // extract the string
    }
    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> result;
        Position prev = Position::NONE;
        for (auto cell : ast_.GetCells()) {
            if (cell.IsValid() && !(cell == prev)) {
                result.push_back(cell);
                prev = cell;
            }
        }
        return result;
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    } catch (FormulaException& fe) {
        throw fe;
    }
}