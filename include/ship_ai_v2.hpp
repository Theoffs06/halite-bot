#pragma once

#include "include/util.hpp"
#include "include/behavior_tree.hpp"

namespace ShipAIV2 {
	// Builds and returns the behavior tree for a ship AI.
	// The tree is static to avoid rebuilding it every time the function is called.
	BehaviorTree::Selector<Payload> GetBehaviorTree();

	// --- Leaf Nodes ---
	class IsFull : public BehaviorTree::Leaf<Payload> {
	public:
		IsFull(BehaviorTree::Node<Payload>* parent);
	};

	class ReturnToBase : public BehaviorTree::Leaf<Payload> {
	public:
		ReturnToBase(BehaviorTree::Node<Payload>* parent);
	};

	class IsOnRichCell : public BehaviorTree::Leaf<Payload> {
	public:
		IsOnRichCell(BehaviorTree::Node<Payload>* parent);
	};

	class CollectHalite : public BehaviorTree::Leaf<Payload> {
	public:
		CollectHalite(BehaviorTree::Node<Payload>* parent);
	};

	class MoveToBestHalite : public BehaviorTree::Leaf<Payload> {
	public:
		MoveToBestHalite(BehaviorTree::Node<Payload>* parent);
	};
} // namespace ShipAIV2
