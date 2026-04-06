#include "include/shipyard_ai.hpp"

// --- Behavior Tree Construction ---

BehaviorTree::Selector<ShipyardPayload> ShipyardAI::GetBehaviorTree() {
	// Root node: selects between spawning or doing nothing
	static BehaviorTree::Selector<ShipyardPayload> root(nullptr);

	// Subtree: checks if we can spawn and, if so, spawns a new ship
	static BehaviorTree::Sequencer<ShipyardPayload> spawnSequencer(&root);
	static CanSpawn canSpawn(&spawnSequencer);
	static SpawnShip spawnShip(&spawnSequencer);

 	return root;
}

// --- Behavior Tree Nodes ---

// --- CanSpawn Node ---
// This node checks if the shipyard can spawn a new ship and, if so, adds the spawn command to the command queue.
ShipyardAI::CanSpawn::CanSpawn(BehaviorTree::Node<ShipyardPayload>* parent) : Leaf(parent) {
	this->m_evaluation = [&](const ShipyardPayload& payload) {
		const auto& game = payload.game;
		const auto& shipyard = payload.shipyard;

		const bool hasEnoughHaliteForNewShip = game.me->halite >= hlt::constants::SHIP_COST + SHIP_COST_OFFSET;
		const bool hasReachedMaxShips = game.me->ships.size() < MAX_SHIP;
		const bool isShipyardFree = !game.game_map->at(shipyard)->is_occupied();

		if (hasEnoughHaliteForNewShip && hasReachedMaxShips && isShipyardFree) {
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}

// --- SpawnShip Node ---
// This node spawns a new ship from the shipyard and adds the spawn command to the command queue.
ShipyardAI::SpawnShip::SpawnShip(BehaviorTree::Node<ShipyardPayload>* parent) : Leaf(parent) {
	this->m_evaluation = [&](const ShipyardPayload& payload) {
		const auto& shipyard = payload.shipyard;

		payload.commands.push_back(shipyard->spawn());
		return BehaviorTree::NodeState::Running;
	};
}
