#include "json_builder.h"
#include <exception>
#include <variant>
#include <utility>

using namespace std::literals;

namespace json {

Builder::Builder()
    : root_()
    , nodes_stack_{&root_}
{}

Builder::Builder(Node& node)
    : root_(node)
    , nodes_stack_{&root_}
{}


Node Builder::Build() {
    if (!nodes_stack_.empty()) {
        throw std::logic_error("Attempt to build JSON which isn't finalized"s);
    }
    return std::move(root_);
}

DictKeyContext Builder::Key(std::string key) {
    Node::Value& host_value = GetCurrentValue();
    
    if (!std::holds_alternative<Dict>(host_value)) {
        throw std::logic_error("Key() outside a dict"s);
    }

    auto val = &std::get<Dict>(host_value)[std::move(key)];
    nodes_stack_.emplace_back(std::move(val));
    return DictKeyContext(*this);
}

Builder& Builder::Value(Node::Value value) {
    AddObject(std::move(value), /* one_shot */ true);
    return *this;
}

DictItemContext Builder::StartDict() {
    AddObject(Dict{}, /* one_shot */ false);
    return DictItemContext(*this);
}

StartArrayContext Builder::StartArray() {
    AddObject(Array{}, /* one_shot */ false);
    return StartArrayContext(*this);
}

Builder& Builder::EndDict() {
    if (!std::holds_alternative<Dict>(GetCurrentValue())) {
        throw std::logic_error("EndDict() outside a dict"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder& Builder::EndArray() {
    if (!std::holds_alternative<Array>(GetCurrentValue())) {
        throw std::logic_error("EndDict() outside an array"s);
    }
    nodes_stack_.pop_back();
    return *this;
}
    
// Current value can be:
// * Dict, when .Key().Value() or EndDict() is expected
// * Array, when .Value() or EndArray() is expected
// * nullptr (default), when first call or dict Value() is expected

Node::Value& Builder::GetCurrentValue() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Attempt to change finalized JSON"s);
    }
    return nodes_stack_.back()->GetValue();
}

// Tell about this trick
const Node::Value& Builder::GetCurrentValue() const {
    return const_cast<Builder*>(this)->GetCurrentValue();
}

void Builder::AssertNewObjectContext() const {
    if (!std::holds_alternative<std::nullptr_t>(GetCurrentValue())) {
        throw std::logic_error("New object in wrong context"s);
    }
}

void Builder::AddObject(Node::Value value, bool one_shot) {
    Node::Value& host_value = GetCurrentValue();
    if (std::holds_alternative<Array>(host_value)) {
        // Tell about emplace_back
        Node& node = std::get<Array>(host_value).emplace_back(std::move(value));
        if (!one_shot) {
            nodes_stack_.push_back(&node);
        }
    } else {
        AssertNewObjectContext();
        host_value = std::move(value);
        if (one_shot) {
            nodes_stack_.pop_back();
        }
    }
}



DictItemContext DictKeyContext::Value(Node::Value value){
    return DictItemContext(builder_.Value(value));
}
DictItemContext DictKeyContext::StartDict() {
    return builder_.StartDict();
}
StartArrayContext DictKeyContext::StartArray() {
    return builder_.StartArray();
}


DictKeyContext DictItemContext::Key(std::string key){
    return builder_.Key(key);
}
Builder& DictItemContext::EndDict() {
    return builder_.EndDict();
}



StartArrayAndValueContext StartArrayContext::Value(Node::Value value){
    return StartArrayAndValueContext(builder_.Value(value));
}
DictItemContext StartArrayContext::StartDict(){
    return builder_.StartDict();
}
StartArrayContext StartArrayContext::StartArray(){
    return builder_.StartArray();
}
Builder& StartArrayContext::EndArray(){
    return builder_.EndArray();
};

StartArrayContext::StartArrayContext(Builder& builder)
    : builder_{builder}
{};


StartArrayAndValueContext::StartArrayAndValueContext(Builder& builder)    
    : builder_{builder}
{};

StartArrayAndValueContext StartArrayAndValueContext::Value(Node::Value value){
    return StartArrayAndValueContext(builder_.Value(value));
}
DictItemContext StartArrayAndValueContext::StartDict(){
    return builder_.StartDict();
}
StartArrayContext StartArrayAndValueContext::StartArray(){
    return builder_.StartArray();
}
Builder& StartArrayAndValueContext::EndArray(){
    return builder_.EndArray();
}

}  // namespace json