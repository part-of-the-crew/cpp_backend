#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <memory>

#include "json.h"

namespace json {
/*
Key(std::string). При определении словаря задаёт строковое значение ключа для очередной пары ключ-значение. Следующий вызов метода обязательно должен задавать соответствующее этому ключу значение с помощью метода Value или начинать его определение с помощью StartDict или StartArray.
Value(Node::Value). Задаёт значение, соответствующее ключу при определении словаря, очередной элемент массива или, если вызвать сразу после конструктора json::Builder, всё содержимое конструируемого JSON-объекта. Может принимать как простой объект — число или строку — так и целый массив или словарь.
Здесь Node::Value — это синоним для базового класса Node, шаблона variant с набором возможных типов-значений. Смотрите заготовку кода.
StartDict(). Начинает определение сложного значения-словаря. Вызывается в тех же контекстах, что и Value. Следующим вызовом обязательно должен быть Key или EndDict.
StartArray(). Начинает определение сложного значения-массива. Вызывается в тех же контекстах, что и Value. Следующим вызовом обязательно должен быть EndArray или любой, задающий новое значение: Value, StartDict или StartArray.
EndDict(). Завершает определение сложного значения-словаря. Последним незавершённым вызовом Start* должен быть StartDict.
EndArray(). Завершает определение сложного значения-массива. Последним незавершённым вызовом Start* должен быть StartArray.
Build(). Возвращает объект json::Node, содержащий JSON, описанный предыдущими вызовами методов. К этому моменту для каждого Start* должен быть вызван соответствующий End*. При этом сам объект должен быть определён, то есть вызов json::Builder{}.Build() недопустим.
*/

class Builder {
    using Node = json::Node;

    mutable Node root_;  // Final built JSON
    std::vector<Node*> node_stack_;  // Stack of current container contexts
    std::string pending_key_;        // Stores current key when inside a dict
    bool key_pending_ = false;       // Flag: a key has been set, waiting for its value
    bool built_ = false;             // Flag: Build() has already been called

public:
    Builder& Value(Node::Value val) {
        Node node{val};

        if (node_stack_.empty()) {
            if (built_) throw std::logic_error("Build() already called");
            root_ = std::move(node);
            built_ = true;
        } else {
            Node* current = node_stack_.back();
            if (current->IsArray()) {
                current->AsArray().emplace_back(std::move(node));
            } else if (current->IsDict()) {
                if (!key_pending_) throw std::logic_error("Key() not called before Value()");
                current->AsDict()[pending_key_] = std::move(node);
                key_pending_ = false;
            } else {
                throw std::logic_error("Invalid state for Value()");
            }
        }
        return *this;
    }

    Builder& Key(std::string key) {
        if (node_stack_.empty() || !node_stack_.back()->IsDict()) {
            throw std::logic_error("Key() called outside of dict");
        }
        if (key_pending_) {
            throw std::logic_error("Previous key missing value");
        }
        pending_key_ = std::move(key);
        key_pending_ = true;
        return *this;
    }

    Builder& StartDict() {
        Node new_dict = json::Dict{};

        if (node_stack_.empty()) {
            if (built_) throw std::logic_error("Build() already called");
            root_ = std::move(new_dict);
            node_stack_.push_back(&root_);
            built_ = true;
        } else {
            Node* current = node_stack_.back();
            if (current->IsArray()) {
                current->AsArray().emplace_back(std::move(new_dict));
                Node& ref = current->AsArray().back();
                node_stack_.push_back(&ref);
            } else if (current->IsDict()) {
                if (!key_pending_) throw std::logic_error("Key() not called before StartDict()");
                current->AsDict()[pending_key_] = std::move(new_dict);
                Node& ref = current->AsDict().at(pending_key_);
                node_stack_.push_back(&ref);
                key_pending_ = false;
            } else {
                throw std::logic_error("Invalid context for StartDict()");
            }
        }
        return *this;
    }

    Builder& EndDict() {
        if (node_stack_.empty() || !node_stack_.back()->IsDict()) {
            throw std::logic_error("EndDict() without matching StartDict()");
        }
        node_stack_.pop_back();
        return *this;
    }

    Builder& StartArray() {
        Node new_array = json::Array{};

        if (node_stack_.empty()) {
            if (built_) throw std::logic_error("Build() already called");
            root_ = std::move(new_array);
            node_stack_.push_back(&root_);
            built_ = true;
        } else {
            Node* current = node_stack_.back();
            if (current->IsArray()) {
                current->AsArray().emplace_back(std::move(new_array));
                Node& ref = current->AsArray().back();
                node_stack_.push_back(&ref);
            } else if (current->IsDict()) {
                if (!key_pending_) throw std::logic_error("Key() not called before StartArray()");
                current->AsDict()[pending_key_] = std::move(new_array);
                Node& ref = current->AsDict().at(pending_key_);
                node_stack_.push_back(&ref);
                key_pending_ = false;
            } else {
                throw std::logic_error("Invalid context for StartArray()");
            }
        }
        return *this;
    }

    Builder& EndArray() {
        if (node_stack_.empty() || !node_stack_.back()->IsArray()) {
            throw std::logic_error("EndArray() without matching StartArray()");
        }
        node_stack_.pop_back();
        return *this;
    }

    Node Build() {
        if (!node_stack_.empty()) {
            throw std::logic_error("Unclosed containers");
        }
        if (!built_) {
            throw std::logic_error("Unclosed containers");
        }
        return root_;
    }
};


}  // namespace json