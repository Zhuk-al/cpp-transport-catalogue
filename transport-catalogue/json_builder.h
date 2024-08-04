#pragma once
 
#include "json.h"
#include <stack>
#include <string>
#include <memory>
 
namespace transport_catalogue {
namespace detail {
namespace json {
namespace builder {

class DictValueContext;
class DictItemContext;
class ArrayItemContext;
 
class Builder {
public:
    DictValueContext Key(const std::string& key);
    Builder& Value(const Node::Value& value);
    
    DictItemContext StartDict();
    Builder& EndDict();
    
    ArrayItemContext StartArray();
    Builder& EndArray();
 
    Node Build();
 
private:
    Node root_;
    std::vector<std::unique_ptr<Node>> nodes_stack_;

    Node MakeNode(const Node::Value& value);
    void AddNode(const Node& node);
 
};
 
class BaseContext {
public:
    BaseContext(Builder& builder);
 
    DictValueContext Key(const std::string& key);
    Builder& Value(const Node::Value& value);
    
    DictItemContext StartDict();
    Builder& EndDict();
    
    ArrayItemContext StartArray();
    Builder& EndArray();
 
protected:
    Builder& builder_;
 
};
 
class DictValueContext : public BaseContext {
public:
    DictValueContext(Builder& builder);
 
    DictValueContext Key(const std::string& key) = delete;
 
    BaseContext EndDict() = delete;
    BaseContext EndArray() = delete;
 
    DictItemContext Value(const Node::Value& value);
};
 
class DictItemContext : public BaseContext {
public:
    DictItemContext(Builder& builder);
 
    DictItemContext StartDict() = delete;
 
    ArrayItemContext StartArray() = delete;
    Builder& EndArray() = delete;
 
    Builder& Value(const Node::Value& value) = delete;
};
 
class ArrayItemContext : public BaseContext {
public:
    ArrayItemContext(Builder& builder);
 
    DictValueContext Key(const std::string& key) = delete;
 
    Builder& EndDict() = delete;
 
    ArrayItemContext Value(const Node::Value& value);
};
 
} //end namespace builder
} //end namespace json
} //end namespace detail
} //end namespace transport_catalogue