#include "json_builder.h"
#include <iterator>


using namespace std::string_literals;

namespace json {


Builder &json::Builder::Value(Node::Value value)
{

    if (stack_.back()->IsArray()){
        auto ar = stack_.back()->AsArray();
        if (std::holds_alternative<std::nullptr_t>(value)){
            ar.emplace_back(std::move(*std::make_unique<Node>(Node{std::get<std::nullptr_t>(value)})));
        } else if (std::holds_alternative<Array>(value)){
            throw std::logic_error("Array1 is not properly created"s);
        } else if (std::holds_alternative<Dict>(value)){
            throw std::logic_error("Array2 is not properly created"s);
        } else if (std::holds_alternative<bool>(value)){
            ar.emplace_back(std::move(*std::make_unique<Node>(Node{std::get<bool>(value)})));
        } else if (std::holds_alternative<int>(value)){
            ar.emplace_back(std::move(*std::make_unique<Node>(Node{std::get<int>(value)})));
        } else if (std::holds_alternative<double>(value)){
            ar.emplace_back(std::move(*std::make_unique<Node>(Node{std::get<double>(value)})));
        } else if (std::holds_alternative<std::string>(value)){
            ar.emplace_back(std::move(*std::make_unique<Node>(Node{std::get<std::string>(value)})));
        }
    } else if (stack_.back()->IsDict()){
        ;
    } else {
        if (std::holds_alternative<std::nullptr_t>(value)){
            stack_.emplace_back(std::move(std::make_unique<Node>(Node{std::get<std::nullptr_t>(value)})));
        } else if (std::holds_alternative<bool>(value)){
            stack_.emplace_back(std::move(std::make_unique<Node>(Node{std::get<bool>(value)})));
        } else if (std::holds_alternative<int>(value)){
            stack_.emplace_back(std::move(std::make_unique<Node>(Node{std::get<int>(value)})));
        } else if (std::holds_alternative<double>(value)){
            stack_.emplace_back(std::move(std::make_unique<Node>(Node{std::get<double>(value)})));
        } else if (std::holds_alternative<std::string>(value)){
            stack_.emplace_back(std::move(std::make_unique<Node>(Node{std::get<std::string>(value)})));
        }
    }

    return *this;
}

Builder &Builder::Key(std::string)
{
    return *this;
}

Builder &Builder::StartDict()
{
    root_ = json::Dict{};
    return *this;
}

Builder &Builder::EndDict()
{
    return *this;
}

Builder &Builder::StartArray()
{
    stack_.emplace_back(std::move(std::make_unique<Node>(json::Array{})));
    //root_ = json::Array{};
    return *this;
}

Builder &Builder::EndArray()
{

    if (!stack_.back()->IsArray()){
        throw std::logic_error("Array2 is not properly created"s);
    }
    /*
    for (auto const& el: stack_){
        ar.emplace_back(std::move(*el));
    }
    */
    //json::Array ar{*std::move(stack_.back())};
    root_ = *std::move(stack_.back());
    stack_.pop_back();
    return *this;
}

json::Node Builder::Build()
{
    if (!stack_.empty()){
        throw std::logic_error("Object is not properly created"s);
    }
    return root_;
}

}
/*
Мы рекомендуем хранить в объекте json::Builder следующее состояние:
Node root_; — сам конструируемый объект.
std::vector<Node*> nodes_stack_; — стек указателей на те вершины JSON, которые ещё не построены:
то есть текущее описываемое значение и цепочка его родителей. 
Он поможет возвращаться в нужный контекст после вызова End-методов.
Начните проектирование кода с описания ожидаемого состояния класса 
(в первую очередь — последней вершины в стеке) для каждой точки в цепочке вызовов в примере.
*/