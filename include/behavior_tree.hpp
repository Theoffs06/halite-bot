#pragma once

#include <vector>
#include <functional>

namespace BehaviorTree {

enum class NodeState {
    Running,
    Success,
    Failure,
};

template <typename... T>
class Node {
public:
    Node(Node* parent) : parent(parent) {
        if (parent != nullptr) {
            this->parent->children.push_back(this);
        }
    }

    virtual NodeState Evaluate(const T... payload) = 0;

protected:
    std::vector<Node*> children;
    Node* parent;
};

template <typename... T>
class Sequencer : public Node<T...> {
public:
    Sequencer(Node<T...>* parent) : Node<T...>(parent) {}

    NodeState Evaluate(const T... payload) override {
        for (Node<T...>* child : this->children) {
            NodeState childState = child->Evaluate(payload...);

            switch (childState) {
            case NodeState::Failure:
            case NodeState::Running:
                return childState;

            default:
                break;
            }
        }

        return NodeState::Success;
    }
};

template <typename... T>
class Selector : public Node<T...> {
public:
    Selector(Node<T...>* parent) : Node<T...>(parent) {}

    NodeState Evaluate(const T... payload) override {
        for (Node<T...>* child : this->children) {
            NodeState childState = child->Evaluate(payload...);

            switch (childState) {
            case NodeState::Success:
            case NodeState::Running:
                return childState;

            default:
                break;
            }
        }

        return NodeState::Failure;
    }
};

template <typename... T>
class Leaf : public Node<T...> {
public:
    Leaf(Node<T...>* parent) : Node<T...>(parent), 
	evaluation([](const T...) {
			return NodeState::Failure;
    }) {}

    Leaf(Node<T...>* parent, std::function<NodeState(const T...)> func) 
	: Node<T...>(parent), evaluation(func) {}

    NodeState Evaluate(const T... payload) override {
        return this->evaluation(payload...);
    }

protected:
    std::function<NodeState(const T...)> evaluation;
};

}
