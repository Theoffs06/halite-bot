#pragma once

#include "include/util.hpp"

namespace ShipyardAI {
	// --- Behavior Tree Construction ---

	BehaviorTree::Selector<ShipyardPayload> GetBehaviorTree();

	// --- Behavior Tree Nodes ---

	// Leaf: Checks if the shipyard can spawn a ship.
	class CanSpawn : public BehaviorTree::Leaf<ShipyardPayload> {
	public:
		CanSpawn(BehaviorTree::Node<ShipyardPayload>* parent);
	};

	// Leaf: Spawns a ship from the shipyard.
	class SpawnShip : public BehaviorTree::Leaf<ShipyardPayload> {
	public:
		SpawnShip(BehaviorTree::Node<ShipyardPayload>* parent);
	};
} // namespace ShipyardAI
