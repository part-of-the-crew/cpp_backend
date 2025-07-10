#pragma once

#include <string>
#include <vector>
#include "json.h"

namespace json {


class DictItemContext;
class DictKeyContext;
class ValueContext;
class StartArrayContext;
class StartArrayAndValueContext;

class Builder {
public:
    Builder();
    Builder(Node& node);
    Node Build();
    DictKeyContext Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict();
    //DictItemContext StartDict(Dict &dict);
    StartArrayContext StartArray();
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

class DictKeyContext {
public:
    DictKeyContext(Builder& builder)
        : builder_{builder} 
        {};
    Builder& builder_;

    ValueContext Value(Node::Value value);
    DictItemContext StartDict();
    StartArrayContext StartArray();
};

class DictItemContext {
public:
    DictItemContext(Builder& builder)
        : builder_{builder} 
        {};
    Builder& builder_;

    DictKeyContext Key(std::string key);
    Builder& EndDict();
};

class ValueContext {
public:
    ValueContext(Builder& builder);
    Builder& builder_;

    DictKeyContext Key(std::string key);
    Builder& EndDict();
};

class StartArrayContext {
public:
    StartArrayContext(Builder& builder);
    Builder& builder_;

    StartArrayAndValueContext Value(Node::Value value);
    DictItemContext StartDict();
    StartArrayContext StartArray();
    Builder& EndArray();
};

class StartArrayAndValueContext {
public:
    StartArrayAndValueContext(Builder& builder);
    Builder& builder_;

    StartArrayAndValueContext Value(Node::Value value);
    DictItemContext StartDict();
    StartArrayContext StartArray();
    Builder& EndArray();
};


}  // namespace json