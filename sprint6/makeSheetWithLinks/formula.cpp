#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, const FormulaError& fe) {
    return output << "#ARITHM!";
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression)
    try
        : ast_(ParseFormulaAST(expression)) {
    } catch (const FormulaException& error) {
        throw error;
    }
    
    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute(sheet);
        } catch (FormulaError error){
            return error;
        }
    }

    std::string GetExpression() const override {
        std::ostringstream out;      // string-based output stream
        ast_.PrintFormula(out);      // print the AST into it
        return out.str();            // extract the string
    }
    std::vector<Position> GetReferencedCells() const override {
        //std::vector<Position> vec(ast_.GetCells().begin(), ast_.GetCells().end());
        return {ast_.GetCells().begin(), ast_.GetCells().end()};
    }
private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}