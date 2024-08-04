#include "json_builder.h"
 
namespace transport_catalogue {
namespace detail {
namespace json {
namespace builder {
 
//---------------------BaseContext---------------------
 
BaseContext::BaseContext(Builder& builder) : builder_(builder) {}
 
DictValueContext BaseContext::Key(const std::string& key) {
    return builder_.Key(std::move(key));
}

Builder& BaseContext::Value(const Node::Value& value) {
    return builder_.Value(std::move(value));
}
 
DictItemContext BaseContext::StartDict() {
    return DictItemContext(builder_.StartDict());
}

Builder& BaseContext::EndDict() {
    return builder_.EndDict();
}
 
ArrayItemContext BaseContext::StartArray() {
    return ArrayItemContext(builder_.StartArray());
}

Builder& BaseContext::EndArray() {
    return builder_.EndArray();
}
    
//---------------------DictValueContext----------------
 
DictValueContext::DictValueContext(Builder& builder) : BaseContext(builder) {}
 
DictItemContext DictValueContext::Value(const Node::Value& value) {
    return BaseContext::Value(std::move(value));
}
    
//---------------------DictItemContext-----------------
 
DictItemContext::DictItemContext(Builder& builder) : BaseContext(builder) {}
 
//---------------------ArrayItemContext----------------
 
ArrayItemContext::ArrayItemContext(Builder& builder) : BaseContext(builder) {}
 
ArrayItemContext ArrayItemContext::Value(const Node::Value& value) {
    return BaseContext::Value(std::move(value));
}

//---------------------Builder-------------------------
 
Node Builder::MakeNode(const Node::Value& value) {
    Node node;
 
    if (std::holds_alternative<bool>(value)) {
        bool bool_ = std::get<bool>(value);
        node = Node(bool_);
 
    } else if (std::holds_alternative<int>(value)) {
        int int_ = std::get<int>(value);
        node = Node(int_);
 
    } else if (std::holds_alternative<double>(value)) {
        double double_ = std::get<double>(value);
        node = Node(double_);
 
    } else if (std::holds_alternative<std::string>(value)) {
        std::string str = std::get<std::string>(value);
        node = Node(std::move(str));
 
    } else if (std::holds_alternative<Array>(value)) {
        Array arr = std::get<Array>(value);
        node = Node(std::move(arr));
 
    } else if (std::holds_alternative<Dict>(value)) {
        Dict dictionary = std::get<Dict>(value);
        node = Node(std::move(dictionary));
 
    } else {
        node = Node();
    }
 
    return node;
}
 
void Builder::AddNode(const Node& node) {
    if (nodes_stack_.empty()) {
 
        if (!root_.IsNull()) {
            throw std::logic_error("root has been added");
        }
        root_ = node;

        return; 

    } else {

        if (!nodes_stack_.back()->IsArray()
            && !nodes_stack_.back()->IsString()) { 
            throw std::logic_error("value error");
        }

        if (nodes_stack_.back()->IsArray()) {
            Array arr = nodes_stack_.back()->AsArray();
            arr.emplace_back(node); 
            nodes_stack_.pop_back();
            auto arr_ptr = std::make_unique<Node>(arr);
            nodes_stack_.emplace_back(std::move(arr_ptr));
 
            return;
        }
 
        if (nodes_stack_.back()->IsString()) {
            std::string str = nodes_stack_.back()->AsString();
            nodes_stack_.pop_back();
 
            if (nodes_stack_.back()->IsMap()) {
                Dict dictionary = nodes_stack_.back()->AsMap();
                dictionary.emplace(std::move(str), node);
 
                nodes_stack_.pop_back();
                auto dictionary_ptr = std::make_unique<Node>(dictionary);
                nodes_stack_.emplace_back(std::move(dictionary_ptr));
            }
 
            return;
        }
    }
}
 
DictValueContext Builder::Key(const std::string& key) {
    if (nodes_stack_.empty()) {
        throw std::logic_error("key error");
    }
 
    auto key_ptr = std::make_unique<Node>(key);
 
    if (nodes_stack_.back()->IsMap()) {
        nodes_stack_.emplace_back(std::move(key_ptr));
    }
 
    return DictValueContext(*this);
}
 
Builder& Builder::Value(const Node::Value& value) {
    AddNode(MakeNode(value));
 
    return *this;
}
 
DictItemContext Builder::StartDict() {
    nodes_stack_.emplace_back(std::move(std::make_unique<Node>(Dict())));
 
    return DictItemContext(*this);
}
 
Builder& Builder::EndDict() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("expected dictionary to end");
    }
 
    Node node = *nodes_stack_.back();
 
    if (!node.IsMap()) {
        throw std::logic_error("object isn't dictionary");
    }
 
    nodes_stack_.pop_back();
    AddNode(node);
 
    return *this;
}
 
ArrayItemContext Builder::StartArray() {
    nodes_stack_.emplace_back(std::move(std::make_unique<Node>(Array())));
 
    return ArrayItemContext(*this);
}
 
Builder& Builder::EndArray() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("expected dictionary to end");
    }
 
    Node node = *nodes_stack_.back();
 
    if (!node.IsArray()) {
        throw std::logic_error("object isn't array");
    }
 
    nodes_stack_.pop_back();
    AddNode(node);
 
    return *this;
}
 
Node Builder::Build() {
    if (root_.IsNull()) {
        throw std::logic_error("empty json");
    }
 
    if (!nodes_stack_.empty()) {
        throw std::logic_error("invalid json");
    }
 
    return root_;
}
 
} //end namespace builder
} //end namespace json
} //end namespace detail
} //end namespace transport_catalogue