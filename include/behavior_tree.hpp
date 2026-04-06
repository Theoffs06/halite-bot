#pragma once

#include <vector>
#include <functional>

namespace BehaviorTree {
	// Possible states of a behavior tree node.
	enum class NodeState {
		Running, // The node is still running and has not yet succeeded or failed.
		Success, // The node has succeeded.
		Failure, // The node has failed.
	};
	
	// Base class for all nodes in the behavior tree.
	template <typename... T>
	class Node {
	public:
		// Constructs a node.
		// parent: Pointer to the parent node (nullptr for root nodes).
	    Node(Node* parent) : m_parent(parent) {
	        if (parent != nullptr) {
	            this->m_parent->m_children.push_back(this);
	        }
	    }

		// Evaluates the node and returns its state.
		// payload...: Arguments passed to the evaluation function.
		// Returns: The result of the evaluation.
		virtual NodeState Evaluate(const T... payload) = 0;

	protected:
	    std::vector<Node*> m_children; // Child nodes.
	    Node* m_parent;				 // Parent node.
	};

	// Composite node that runs children sequentially.
	// A Sequencer returns Success only if all children return Success.
	// If any child returns Failure or Running, it stops and returns that state.
	template <typename... T>
	class Sequencer : public Node<T...> {
	public:
		// Constructs a Sequencer node.
		// parent: Pointer to the parent node.
	    Sequencer(Node<T...>* parent) : Node<T...>(parent) {}

	    NodeState Evaluate(const T... payload) override {
	        for (Node<T...>* child : this->m_children) {
	            NodeState childState = child->Evaluate(payload...);

				if (childState != NodeState::Success) {
					return childState;
				}
	        }

	        return NodeState::Success;
	    }
	};

	// Composite node that runs children until one returns Success or Running.
	// A Selector returns Failure only if all children return Failure.
	// As soon as a child returns Success or Running, it stops and returns that state.
	template <typename... T>
	class Selector : public Node<T...> {
	public:
		// Constructs a Selector node.
		// parent: Pointer to the parent node.
	    Selector(Node<T...>* parent) : Node<T...>(parent) {}

	    NodeState Evaluate(const T... payload) override {
	        for (Node<T...>* child : this->m_children) {
	            NodeState childState = child->Evaluate(payload...);

				if (childState != NodeState::Failure) {
					return childState;
				}
	        }

	        return NodeState::Failure;
	    }
	};

	// Leaf node that executes a custom function.
	// A Leaf node cannot have children and executes a provided function to determine its state.
	template <typename... T>
	class Leaf : public Node<T...> {
	public:
		// Constructs a Leaf node with a default function (returns Failure).
		// parent: Pointer to the parent node.
	    Leaf(Node<T...>* parent) : Node<T...>(parent), 
			m_evaluation([](const T...) { return NodeState::Failure; }) {}

		// Constructs a Leaf node with a custom evaluation function.
		// parent: Pointer to the parent node.
		// func: Function to execute when the node is evaluated.
	    Leaf(Node<T...>* parent, std::function<NodeState(const T...)> func) 
			: Node<T...>(parent), m_evaluation(func) {}

	    NodeState Evaluate(const T... payload) override {
	        return this->m_evaluation(payload...);
	    }

	protected:
	    std::function<NodeState(const T...)> m_evaluation; // Function executed to evaluate the node.
	};
} // namespace BehaviorTree
