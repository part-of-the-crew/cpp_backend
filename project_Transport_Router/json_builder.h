#pragma once

#include <string>
#include <vector>
#include "json.h"

namespace json {


class DictKeyContext;
class DictItemContext;//
class StartArrayAndValueContext;//*

class Builder {
public:
    Builder();
    Builder(Node& node);
    Node Build();
    DictKeyContext Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict();
    StartArrayAndValueContext StartArray();
    Builder& EndDict();
    Builder& EndArray();

private:
    Node root_;
    std::vector<Node*> nodes_stack_;

    Node::Value& GetCurrentValue();
    const Node::Value& GetCurrentValue() const;
    
    void AssertNewObjectContext() const;
    void AddObject(Node::Value value, bool one_shot);
};

class BasicContext {
protected:
    BasicContext(Builder& builder)
        : builder_{builder} 
        {};
    Builder& builder_;
};

class DictKeyContext : public BasicContext {
public:
    DictKeyContext(Builder& builder) : BasicContext(builder) {}
    DictItemContext Value(Node::Value value);
    DictItemContext StartDict();
    StartArrayAndValueContext StartArray();
};

class DictItemContext : public BasicContext {
public:
    DictItemContext(Builder& builder) : BasicContext(builder) {}
    DictKeyContext Key(std::string key);
    Builder& EndDict();
};


class StartArrayAndValueContext : public BasicContext {
public:
    StartArrayAndValueContext(Builder& builder) : BasicContext(builder) {}
    StartArrayAndValueContext Value(Node::Value value);
    DictItemContext StartDict();
    StartArrayAndValueContext StartArray();
    Builder& EndArray();
};


}  // namespace json