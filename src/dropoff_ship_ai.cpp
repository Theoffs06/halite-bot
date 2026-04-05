#include "include/dropoff_ship_ai.hpp"

// --- DropoffShipAi::TransformIntoDropoff ---
// Checks if the ship has enough halite to transform into a dropoff point.
// If yes, it issues the command to transform and returns Success.
DropoffShipAi::TransformIntoDropoff::TransformIntoDropoff(BehaviorTree::Node<Payload>* parent): Leaf(parent) {
	this->evaluation = [&](const Payload& payload) {
		// Total halite available: ship's halite + player's halite + halite at current 
		unsigned int availableHalite = payload.ship->halite + payload.game.me->halite + payload.game.game_map->at(payload.ship->position)->halite;
		
		// If enough halite is available, transform into a dropoff and return Success
		if (availableHalite >= 4000) {
			payload.commands.push_back(payload.ship->make_dropoff());
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}

// --- DropoffShipAi::MoveToBigHaliteSpot ---
// Moves the ship to the spot with the most halite on the map.
// If multiple spots have the same halite, it picks the closest one.
DropoffShipAi::MoveToBigHaliteSpot::MoveToBigHaliteSpot(BehaviorTree::Node<Payload>* parent): Leaf(parent) {
	this->evaluation = [&](const Payload& payload) {
		hlt::Position bestPositionSoFar;
		unsigned int maxHaliteSoFar = 0;
		unsigned int currentDistance = 100000000;

		// Search the entire map for the best halite spot
		for (int y = 0; y < payload.game.game_map->height; y++) {
			for (int x = 0; x < payload.game.game_map->width; x++) {
				hlt::MapCell* cell = payload.game.game_map->at({ x, y });
				
				// Update best spot if current halite is higher
				if (cell->halite > maxHaliteSoFar && !cell->has_structure()) {
					bestPositionSoFar = cell->position;
					maxHaliteSoFar = cell->halite;
				}
				// If halite is equal, pick the closest one
				else if (cell->halite == maxHaliteSoFar) {
					int distance = payload.game.game_map->calculate_distance(payload.ship->position, { x ,y });
					
					if (distance < currentDistance) {
						currentDistance = distance;
						bestPositionSoFar = cell->position;
					}
				}
			}
		}

		// Navigate to the best spot and send the move command
		hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, bestPositionSoFar);
		payload.commands.push_back(payload.ship->move(direction));
		return BehaviorTree::NodeState::Running;
	};
}

// --- DropoffShipAi::IsOnBigHaliteSpot ---
// Checks if the ship is on a spot with a large amount of halite (>= 800).
DropoffShipAi::IsOnBigHaliteSpot::IsOnBigHaliteSpot(BehaviorTree::Node<Payload>* parent): Leaf(parent) {
	this->evaluation = [&](const Payload& payload) {
		hlt::Position pos = payload.ship->position;

		// Sum halite in a 5x5 area around the ship
		unsigned int totalHalite = 0;
		for (int y = pos.y - 2; y < pos.y + 2; y++) {
			for (int x = pos.x - 2; x < pos.x + 2; x++) {
				totalHalite += payload.game.game_map->at({ x, y })->halite;
			}
		}

		// If halite at current position is >= 800, return Success
		if (payload.game.game_map->at(payload.ship->position)->halite >= 800) {
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}
