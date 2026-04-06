#include "include/ship_ai.hpp"

// --- Behavior Tree Construction ---

BehaviorTree::Selector<ShipPayload> ShipAI::GetMinerBehaviorTree() {
	// Root node: selects between mining or depositing
	static BehaviorTree::Selector<ShipPayload> root(nullptr);

	// SubBehavior: dropoff ship behavior (going back to the shipyard to drop off halite when the game is about to end or when on a good spot to drop off)
	static BehaviorTree::Selector<ShipPayload> dropoffShipAi = DropoffShipAi::GetBehaviorTree(&root);

	// Subtree: depositing end game behavior (going back to the shipyard to drop off halite when the game is about to end)
	static BehaviorTree::Sequencer<ShipPayload> depositingEndGameSubtree(&root);
	static ShouldDeposit shouldDepositEndGame(&depositingEndGameSubtree);
	static GoDeposit goDepositEndGame(&depositingEndGameSubtree);

	// Subtree: depositing behavior (going back to the shipyard to drop off halite)
	static BehaviorTree::Sequencer<ShipPayload> depositingSubtree(&root);
	static ShouldDeposit shouldDeposit(&depositingSubtree);
	static GoDeposit goDeposit(&depositingSubtree);

	// Subtree: mining behavior (collecting halite)
	static BehaviorTree::Sequencer<ShipPayload> miningSubtree(&root);
	static IsNotFull isNotFull(&miningSubtree);
	static HaliteHere haliteHere(&miningSubtree);
	static CollectHalite collectHalite(&miningSubtree);

	// Leaf: move to a cool halite spot
	static MoveToBestHaliteSpot moveToBestHalite(&root);

	return root;
}

BehaviorTree::Selector<ShipPayload> ShipAI::GetExplorerBehaviorTree() {
	// Root node: selects between mining or depositing
	static BehaviorTree::Selector<ShipPayload> root(nullptr);

	// SubBehavior: dropoff ship behavior (going back to the shipyard to drop off halite when the game is about to end or when on a good spot to drop off)
	static BehaviorTree::Selector<ShipPayload> dropoffShipAi = DropoffShipAi::GetBehaviorTree(&root);

	// Subtree: depositing end game behavior (going back to the shipyard to drop off halite when the game is about to end)
	static BehaviorTree::Sequencer<ShipPayload> depositingEndGameSubtree(&root);
	static ShouldDeposit shouldDepositEndGame(&depositingEndGameSubtree);
	static GoDeposit goDepositEndGame(&depositingEndGameSubtree);

	// Subtree: depositing behavior (going back to the shipyard to drop off halite)
	static BehaviorTree::Sequencer<ShipPayload> depositingSubtree(&root);
	static ShouldDeposit shouldDeposit(&depositingSubtree);
	static GoDeposit goDeposit(&depositingSubtree);

	// Leaf: move to a cool halite spot
	static MoveToBestHaliteSpot moveToBestHalite(&root);

	return root;
}

// --- Behavior Tree Nodes ---

// --- EndShouldDeposit Node ---
/* 
 * This node checks if the game is in the endgame phase and if the ship has enough halite to justify returning to the shipyard to deposit. 
 * If both conditions are met, it returns Success; otherwise, it returns Failure.
 */
ShipAI::EndShouldDeposit::EndShouldDeposit(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->evaluation = [&](const ShipPayload& payload) {
		if (payload.game.turn_number >= END_TURN && payload.ship->halite >= END_GO_HOME_HALITE) {
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}

// --- ShouldDeposit Node ---
/* 
 * This node checks if the ship has enough halite to justify returning to the shipyard to deposit. 
 * If the ship's halite is greater than or equal to the GO_HOME_HALITE threshold, it returns Success; otherwise, it returns Failure.
 */
ShipAI::ShouldDeposit::ShouldDeposit(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->evaluation = [&](const ShipPayload& payload) {
		const std::unique_ptr<hlt::GameMap>& gameMap = payload.game.game_map;
		std::shared_ptr<hlt::Shipyard> shipyard = payload.game.me->shipyard;
		std::shared_ptr<hlt::Ship> ship = payload.ship;

		// Find the closest dropoff point (shipyard or existing dropoffs).
		unsigned int bestDistanceSoFar = 1000000000;
		hlt::Position closestDropoffSoFar;

		// Check existing dropoffs.
		for (const auto& dropoff : payload.game.me->dropoffs) {
			unsigned int distance = gameMap->calculate_distance(dropoff.second->position, ship->position);

			if (distance < bestDistanceSoFar) {
				bestDistanceSoFar = distance;
				closestDropoffSoFar = dropoff.second->position;
			}
		}

		// Compare with shipyard distance.
		if (gameMap->calculate_distance(shipyard->position, ship->position) < bestDistanceSoFar) {
			closestDropoffSoFar = shipyard->position;
		}

		const unsigned int cost = payload.moveManager.GetCostPath(gameMap, ship, closestDropoffSoFar);
		if (payload.ship->halite >= cost + GO_HOME_HALITE) {
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}

// --- GoDeposit Node ---
/* 
 * This node commands the ship to move towards the shipyard to deposit halite. 
 * It uses the game map's naive navigation to find the direction and marks the target cell as unsafe.
 */
ShipAI::GoDeposit::GoDeposit(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->evaluation = [&](const ShipPayload& payload) {
		const std::unique_ptr<hlt::GameMap>& gameMap = payload.game.game_map;
		std::shared_ptr<hlt::Shipyard> shipyard = payload.game.me->shipyard;
		std::shared_ptr<hlt::Ship> ship = payload.ship;

		// Find the closest dropoff point (shipyard or existing dropoffs).
		unsigned int bestDistanceSoFar = 1000000000;
		hlt::Position closestDropoffSoFar;

		// Check existing dropoffs.
		for (const auto& dropoff : payload.game.me->dropoffs) {
			unsigned int distance = gameMap->calculate_distance(dropoff.second->position, ship->position);

			if (distance < bestDistanceSoFar) {
				bestDistanceSoFar = distance;
				closestDropoffSoFar = dropoff.second->position;
			}
		}

		// Compare with shipyard distance.
		if (gameMap->calculate_distance(shipyard->position, ship->position) < bestDistanceSoFar) {
			closestDropoffSoFar = shipyard->position;
		}

		hlt::Direction direction = payload.moveManager.GetNextDirection(gameMap, ship, closestDropoffSoFar);

		const bool isShipHaliteTooLowToMoveEfficiently = ship->halite < gameMap->at(ship)->halite * (1.0 / hlt::constants::MOVE_COST_RATIO);
		if (isShipHaliteTooLowToMoveEfficiently || direction == hlt::Direction::STILL) {
			gameMap->at(ship)->mark_unsafe(ship);
			payload.commands.push_back(ship->stay_still());
			return BehaviorTree::NodeState::Running;
		}

		// Mark the target cell as unsafe to prevent other ships from moving into it.
		gameMap->at(ship->position.directional_offset(direction))->mark_unsafe(ship);

		payload.commands.push_back(ship->move(direction));
		return BehaviorTree::NodeState::Running;
	};
}

// --- HaliteHere Node ---
/* 
 * This node checks if there is a significant amount of halite in the current cell. 
 * If there is, it returns Success; otherwise, it returns Failure.
 */
ShipAI::HaliteHere::HaliteHere(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->evaluation = [&](const ShipPayload& payload) {
		if (payload.game.game_map->at(payload.ship)->halite > INTERESTING_HALITE_CELL) {
			return BehaviorTree::NodeState::Success;
		}

		return BehaviorTree::NodeState::Failure;
	};
}

// --- IsNotFull Node ---
/* 
 * This node checks if the ship is not full of halite. 
 * If the ship is full, it returns Failure; otherwise, it returns Success.
 */
ShipAI::IsNotFull::IsNotFull(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->evaluation = [&](const ShipPayload& payload) {
		if (payload.ship->is_full()) {
			return BehaviorTree::NodeState::Failure;
		}

		return BehaviorTree::NodeState::Success;
	};
}

// --- CollectHalite Node ---
/* 
 * This node commands the ship to stay still and collect halite from the current cell. 
 * It marks the cell as unsafe to prevent other ships from moving into it.
 */
ShipAI::CollectHalite::CollectHalite(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->evaluation = [&](const ShipPayload& payload) {
		std::shared_ptr<hlt::Ship> ship = payload.ship;

		payload.game.game_map->at(payload.ship)->mark_unsafe(ship);
		payload.commands.push_back(ship->stay_still());
		return BehaviorTree::NodeState::Running;
	};
}

// --- Leaf Nodes ---

// --- MoveToBestHalite Node ---
// This node would contain logic to move the ship towards the best halite on the map.
ShipAI::MoveToBestHaliteSpot::MoveToBestHaliteSpot(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->evaluation = [&](const ShipPayload& payload) {
		const std::unique_ptr<hlt::GameMap>& gameMap = payload.game.game_map;
		std::shared_ptr<hlt::Ship> ship = payload.ship;

		// Find the best halite spot on the map for this ship.
		const hlt::Position bestSpotPosition = payload.spotManager.GetTheBestHaliteSpot(gameMap, ship);

		// Get the direction to navigate towards the best halite spot.
		hlt::Direction direction = payload.moveManager.GetNextDirection(gameMap, ship, bestSpotPosition);

		const bool isShipHaliteTooLowToMoveEfficiently = ship->halite < gameMap->at(ship)->halite * (1.0 / hlt::constants::MOVE_COST_RATIO);
		if (isShipHaliteTooLowToMoveEfficiently || direction == hlt::Direction::STILL) {
			gameMap->at(ship)->mark_unsafe(ship);
			payload.commands.push_back(ship->stay_still());
			return BehaviorTree::NodeState::Running;
		}

		// Mark the target cell as unsafe to prevent other ships from moving into it.
		gameMap->at(ship->position.directional_offset(direction))->mark_unsafe(ship);

		// Command the ship to move in the chosen direction.
		payload.commands.push_back(ship->move(direction));
		return BehaviorTree::NodeState::Running;
	};
}