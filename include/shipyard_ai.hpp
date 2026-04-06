#pragma once

#include "include/util.hpp"
#include "include/behavior_tree.hpp"

namespace ShipyardAI {
	// --- Behavior Tree Construction ---

	// Constructs and returns the behavior tree for the shipyard.
	BehaviorTree::Selector<ShipyardPayload> GetBehaviorTree();

	// --- Behavior Tree Nodes ---

	// --- CanSpawn Node ---
	// This node checks if the shipyard can spawn a new ship and, if so, adds the spawn command to the command queue.
	class CanSpawn : public BehaviorTree::Leaf<ShipyardPayload> {
	public:
		CanSpawn(BehaviorTree::Node<ShipyardPayload>* parent);
	};

	// --- SpawnShip Node ---
	// This node spawns a new ship from the shipyard and adds the spawn command to the command queue.
	class SpawnShip : public BehaviorTree::Leaf<ShipyardPayload> {
	public:
		SpawnShip(BehaviorTree::Node<ShipyardPayload>* parent);
	};
} // namespace ShipyardAI
