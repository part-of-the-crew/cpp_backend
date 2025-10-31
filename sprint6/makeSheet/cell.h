#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
    Cell();
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

private:
// Base implementation for all cell types
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual std::string GetText() const = 0;
        virtual Value GetValue() const = 0;
    };

    // Empty cell implementation
    class EmptyImpl : public Impl {
    public:
        std::string GetText() const override;
        Value GetValue() const override;
    };

    // Text cell implementation
    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string text);
        std::string GetText() const override;
        Value GetValue() const override;
    private:
        std::string text_;
    };

    // Formula cell implementation
    class FormulaImpl : public Impl {
    public:
        explicit FormulaImpl(std::string expression);
        std::string GetText() const override;
        Value GetValue() const override;
    private:
        std::unique_ptr<FormulaInterface> formula_;
    };
    std::unique_ptr<Impl> impl_;
};