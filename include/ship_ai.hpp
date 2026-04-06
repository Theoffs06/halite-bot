#pragma once

#include "include/util.hpp"
#include "include/behavior_tree.hpp"

namespace ShipAI {
	// --- Behavior Tree Construction ---

	BehaviorTree::Selector<ShipPayload> GetBehaviorTree();

	// --- Behavior Tree Nodes ---

	class MoveToBestHaliteSpot : public BehaviorTree::Leaf<ShipPayload> {
	public:
		MoveToBestHaliteSpot(BehaviorTree::Node<ShipPayload>* parent);
	};

	// --- Utility Functions ---

	hlt::Direction UnblockShip(const std::unique_ptr<hlt::GameMap>& gameMap, const hlt::Direction direction, const hlt::Position& goal);
} // namespace ShipAI
