#include "include/ship_ai.hpp"

// --- ShipAI::HaliteHere ---
// Checks if there is a significant amount of halite (>50) at the ship's current position.
ShipAI::HaliteHere::HaliteHere(BehaviorTree::Node<Payload>* parent) : Leaf(parent) {
	this->evaluation = [&](const Payload& payload) {
		if (payload.game.game_map->at(payload.ship)->halite > 50) {
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}

// --- ShipAI::IsNotFull ---
// Checks if the ship's cargo is not full.
ShipAI::IsNotFull::IsNotFull(BehaviorTree::Node<Payload>* parent): Leaf(parent) {
	this->evaluation = [&](const Payload& payload) {
		if (payload.ship->is_full()) {
			return BehaviorTree::NodeState::Failure;
		}

		return BehaviorTree::NodeState::Success;
	};
}

// --- ShipAI::ShouldDeposit ---
// Determines if the ship should deposit halite at a dropoff point.
// (Cost calculation is commented out for now.)
ShipAI::ShouldDeposit::ShouldDeposit(BehaviorTree::Node<Payload>* parent): Leaf(parent) {
	this->evaluation = [&](const Payload& payload) {
		unsigned int distanceToBase = payload.game.game_map->calculate_distance(
			payload.ship->position, payload.game.me->shipyard->position
		);

		auto moves = payload.game.game_map->get_unsafe_moves(
			payload.ship->position, payload.game.me->shipyard->position
		);

		unsigned int cost = 0;
		/*
		// (Optional) Calculate the cost to reach the dropoff point.
		hlt::Position currentPos = payload.ship->position;
		for (hlt::Direction move : moves) {
			// ... (cost calculation logic)
		}
		*/

		// If the ship has enough halite to cover the cost and a buffer (800), return Success.
		if (payload.ship->halite >= cost + 800) {
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}

// --- ShipAI::GoDeposit ---
// Orders the ship to move toward the closest dropoff point (shipyard or existing dropoffs).
ShipAI::GoDeposit::GoDeposit(BehaviorTree::Node<Payload>* parent): Leaf(parent) {
	this->evaluation = [&](const Payload& payload) {
		// Find the closest dropoff point (shipyard or existing dropoffs).
		unsigned int bestDistanceSoFar = 1000000000;
		hlt::Position closestDropoffSoFar;

		// Check existing dropoffs.
		for (const auto& dropoff : payload.game.me->dropoffs) {
			unsigned int distance = payload.game.game_map->calculate_distance(
				dropoff.second->position, payload.ship->position
			);
			
			if (distance < bestDistanceSoFar) {
				bestDistanceSoFar = distance;
				closestDropoffSoFar = dropoff.second->position;
			}
		}

		// Compare with shipyard distance.
		if (payload.game.game_map->calculate_distance(payload.game.me->shipyard->position, payload.ship->position) < bestDistanceSoFar) {
			closestDropoffSoFar = payload.game.me->shipyard->position;
		}

		// Navigate to the closest dropoff point.
		hlt::Position shipPos = payload.ship->position;
		hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, closestDropoffSoFar);
		
		// Handle STILL case: try to move in an empty adjacent cell.
		if (direction == hlt::Direction::STILL) {
			if (payload.game.game_map->at({ shipPos.x + 1, shipPos.y })->is_empty()) {
				direction = hlt::Direction::EAST;
			}
			else if (payload.game.game_map->at({ shipPos.x - 1, shipPos.y })->is_empty()) {
				direction = hlt::Direction::WEST;
			}
			else if (payload.game.game_map->at({ shipPos.x, shipPos.y + 1 })->is_empty()) {
				direction = hlt::Direction::SOUTH;
			}
			else if (payload.game.game_map->at({ shipPos.x, shipPos.y - 1 })->is_empty()) {
				direction = hlt::Direction::NORTH;
			}
		}

		// Send the move command.
		payload.commands.push_back(payload.ship->move(direction));
		return BehaviorTree::NodeState::Running;
	};
}

// --- ShipAI::MoveToCoolHaliteSpot ---
// Orders the ship to move to a spot with the highest halite in a local area.
ShipAI::MoveToCoolHaliteSpot::MoveToCoolHaliteSpot(BehaviorTree::Node<Payload>* parent): Leaf(parent) {
	this->evaluation = [&](const Payload& payload) {
		hlt::Position bestPositionSoFar;
		unsigned int maxHaliteSoFar = 0;

		// Search a 13x9 area around the ship for the best halite spot.
		hlt::Position pos = payload.ship->position;
		for (unsigned int y = pos.y - 8; y < pos.y + 5; y++) {
			for (unsigned int x = pos.x - 5; x < pos.x + 5; x++) {
				hlt::MapCell* cell = payload.game.game_map->at({ (int) x, (int) y });
				
				if (cell->halite > maxHaliteSoFar && cell->is_empty()) {
					bestPositionSoFar = cell->position;
					maxHaliteSoFar = cell->halite;
				}
			}
		}
		// Navigate to the best spot.
		hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, bestPositionSoFar);
		
		// Handle STILL case: try to move in an empty adjacent cell.
		if (direction == hlt::Direction::STILL) {
			if (payload.game.game_map->at({ bestPositionSoFar.x + 1, bestPositionSoFar.y })->is_empty()) {
				direction = hlt::Direction::EAST;
			}
			else if (payload.game.game_map->at({ bestPositionSoFar.x - 1, bestPositionSoFar.y })->is_empty()) {
				direction = hlt::Direction::WEST;
			}
			else if (payload.game.game_map->at({ bestPositionSoFar.x, bestPositionSoFar.y + 1 })->is_empty()) {
				direction = hlt::Direction::SOUTH;
			}
			else if (payload.game.game_map->at({ bestPositionSoFar.x, bestPositionSoFar.y - 1 })->is_empty()) {
				direction = hlt::Direction::NORTH;
			}
		}

		// Send the move command.
		payload.commands.push_back(payload.ship->move(direction));
		return BehaviorTree::NodeState::Running;
	};
}

// --- ShipAI::CollectHalite ---
// Orders the ship to stay still and collect halite.
ShipAI::CollectHalite::CollectHalite(BehaviorTree::Node<Payload>* parent): Leaf(parent) {
	this->evaluation = [&](const Payload& payload) {
		// Stay still to collect halite.
		payload.commands.push_back(payload.ship->stay_still());
		return BehaviorTree::NodeState::Running;
	};
}

// --- Behavior Tree Construction ---

BehaviorTree::Selector<Payload> ShipAI::GetBehaviorTree()
{
	// Root node: selects between mining or depositing
	static BehaviorTree::Selector<Payload> root(nullptr);

	// Subtree for dropoff ship AI (decides whether to transform into a dropoff point)
	static BehaviorTree::Selector<Payload> dropoffShipAi = DropoffShipAi::GetBehaviorTree(&root);

	// Subtree: go deposit halite if the ship has enough
	static BehaviorTree::Sequencer<Payload> goDepositIfEnoughHalite(&root);
	static ShouldDeposit shouldDeposit(&goDepositIfEnoughHalite);
	static GoDeposit goDeposit(&goDepositIfEnoughHalite);

	// Subtree: mining behavior (collecting halite)
	static BehaviorTree::Sequencer<Payload> mining(&root);
	static IsNotFull isNotFull(&mining);
	static HaliteHere haliteHere(&mining);
	static CollectHalite collectHalite(&mining);

	// Leaf: move to a cool halite spot if no halite is present
	static MoveToCoolHaliteSpot moveToCoolHaliteSpot(&root);

	return root;
}
