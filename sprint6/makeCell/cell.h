#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
    Cell();
    ~Cell();

    void Set(std::string text);
    void Clear();
    //using Value = std::variant<std::string, double, FormulaError>;
    Value GetValue() const override;
    std::string GetText() const override;

private:

    class Impl {
    public:
        virtual std::string GetText() const = 0;
        virtual Value GetValue() const = 0;
        virtual ~Impl() = default;
    };

    class EmptyImpl : public Impl {
    public:
        std::string GetText() const {
            return std::string {};
        }
        Value GetValue() const {
            return std::string {};
        }
        ~EmptyImpl() = default;
    };

    class TextImpl : public Impl {
    public:
        explicit TextImpl(const std::string& text)
            : text_(text) 
            {}   //

        std::string GetText() const {
            return text_;
        }
        Value GetValue() const {
            std::string result;
            if (text_.front() == ESCAPE_SIGN){
                result = text_.substr(1);
            } else {
                result = text_;
            }
            return result;

        }
        ~TextImpl() = default;
    private:
        std::string text_;     // 
    };

    class FormulaImpl : public Impl {
    public:
        explicit FormulaImpl(std::string text) {
            formula_ = ParseFormula(text.substr(1));
        }
        
        std::string GetText() const {
            return "=" + formula_->GetExpression();
        }
        Value GetValue() const {
            Value value_res;
            FormulaInterface::Value value;
            try {
                value = formula_->Evaluate(); 
            } catch (const FormulaException& error ){
               throw error;
            } 

            if (std::holds_alternative<FormulaError>(value)) {
                return value_res = std::get<FormulaError>(value);
            }
            return value_res = std::get<double>(value);
        }
        ~FormulaImpl() = default;
    private:
        std::unique_ptr<FormulaInterface> formula_;
    };

    std::unique_ptr<Impl> impl_;

};