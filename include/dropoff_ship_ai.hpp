#pragma once

#include "include/util.hpp"
#include "include/behavior_tree.hpp"

namespace DropoffShipAi {
	// --- Behavior Tree Construction ---

	BehaviorTree::Selector<ShipPayload> GetBehaviorTree(BehaviorTree::Node<ShipPayload>* parent = nullptr);

	// --- Behavior Tree Nodes ---

	class TransformIntoDropoff : public BehaviorTree::Leaf<ShipPayload> {
	public:
		TransformIntoDropoff(BehaviorTree::Node<ShipPayload>* parent);
	};

	class IsOnBigHaliteSpot : public BehaviorTree::Leaf<ShipPayload> {
	public:
		IsOnBigHaliteSpot(BehaviorTree::Node<ShipPayload>* parent);
	};
} // namespace DropoffShipAi