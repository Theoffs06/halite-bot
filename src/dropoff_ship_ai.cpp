#include "include/dropoff_ship_ai.hpp"

// --- Behavior Tree Construction ---

BehaviorTree::Selector<ShipPayload> DropoffShipAi::GetBehaviorTree(BehaviorTree::Node<ShipPayload>* parent) {
	// Root node: selects between actions
	static BehaviorTree::Selector<ShipPayload> root(parent);

	// Subtree: decide whether to transform into a dropoff point
	static BehaviorTree::Sequencer<ShipPayload> shouldTransformIntoDropoff(&root);
	static IsOnBigHaliteSpot isOnBigHaliteSpot(&shouldTransformIntoDropoff);
	static TransformIntoDropoff transformIntoDropoff(&shouldTransformIntoDropoff);

	return root;
}

// --- Behavior Tree Nodes ---

// --- TransformIntoDropoff Node ---
/* 
 * Checks if the ship has enough halite to transform into a dropoff point.
 */
DropoffShipAi::TransformIntoDropoff::TransformIntoDropoff(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->m_evaluation = [&](const ShipPayload& payload) {
		unsigned int availableHalite = payload.ship->halite + payload.game.me->halite + payload.game.game_map->at(payload.ship->position)->halite;

		if (availableHalite > hlt::constants::DROPOFF_COST + DROPOFF_COST_OFFSET && !alreadyDropoff) {
			alreadyDropoff = true;

			payload.commands.push_back(payload.ship->make_dropoff());
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}

// --- IsOnBigHaliteSpot Node ---
/*
 * Checks if the ship is on a spot with a large amount of halite (>= 800).
 */
DropoffShipAi::IsOnBigHaliteSpot::IsOnBigHaliteSpot(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->m_evaluation = [&](const ShipPayload& payload) {
		const hlt::Position position = payload.ship->position;

		// Sum halite in a 5x5 area around the ship
		unsigned int totalHalite = 0;
		for (int y = position.y - 2; y < position.y + 2; y++) {
			for (int x = position.x - 2; x < position.x + 2; x++) {
				totalHalite += payload.game.game_map->at({ x, y })->halite;
			}
		}

		// If halite at current position is >= 800, return Success
		if (payload.game.game_map->at(payload.ship->position)->halite >= MIN_HALITE_TO_DROPOFF) {
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}