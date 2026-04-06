#pragma once

#include "include/util.hpp"
#include "include/behavior_tree.hpp"

namespace DropoffShipAi {
	// --- Behavior Tree Construction ---

	BehaviorTree::Selector<ShipPayload> GetBehaviorTree(BehaviorTree::Node<ShipPayload>* parent = nullptr);

	// --- Behavior Tree Nodes ---

	// Leaf node: Transforms the ship into a dropoff point if it's on a big halite spot.
	class TransformIntoDropoff : public BehaviorTree::Leaf<ShipPayload> {
	public:
		TransformIntoDropoff(BehaviorTree::Node<ShipPayload>* parent);
	};

	// Leaf node: Checks if the ship is currently on a big halite spot.
	class IsOnBigHaliteSpot : public BehaviorTree::Leaf<ShipPayload> {
	public:
		IsOnBigHaliteSpot(BehaviorTree::Node<ShipPayload>* parent);
	};
} // namespace DropoffShipAi