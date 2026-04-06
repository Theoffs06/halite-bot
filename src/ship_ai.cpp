#include "include/ship_ai.hpp"

// --- Behavior Tree Construction ---

BehaviorTree::Selector<ShipPayload> ShipAI::GetBehaviorTree() {
	// Root node: selects between mining or depositing
	static BehaviorTree::Selector<ShipPayload> root(nullptr);

	// Leaf: move to a cool halite spot
	static MoveToBestHaliteSpot moveToBestHalite(&root);

	return root;
}

// --- Behavior Tree Nodes ---

// --- MoveToBestHalite Node ---
// This node would contain logic to move the ship towards the best halite on the map.
ShipAI::MoveToBestHaliteSpot::MoveToBestHaliteSpot(BehaviorTree::Node<ShipPayload>* parent) : Leaf(parent) {
	this->evaluation = [&](const ShipPayload& payload) {
		const auto& gameMap = payload.game.game_map;
		const auto& ship = payload.ship;

		hlt::Position bestPosition = payload.ship->position;
		double bestScore = -std::numeric_limits<double>::infinity();
		unsigned int bestDistance = std::numeric_limits<unsigned int>::max();

		for (unsigned int y = 0; y < gameMap->height; ++y) {
			for (unsigned int x = 0; x < gameMap->width; ++x) {
				const auto& cell = gameMap->at({
					(int) x, 
					(int) y
				});

				if (cell->has_structure()) continue;

				const unsigned int distance = gameMap->calculate_distance(ship->position, cell->position);
				const double score = cell->halite - (distance * DISTANCE_PENALTY);

				if (score > bestScore || (score == bestScore && distance < bestDistance)) {
					bestScore = score;
					bestPosition = cell->position;
					bestDistance = distance;
				}
			}
		}

		hlt::Direction direction = gameMap->naive_navigate(ship, bestPosition);
		direction = UnblockShip(gameMap, direction, bestPosition);

		payload.commands.push_back(ship->move(direction));
		return BehaviorTree::NodeState::Running;
	};
}

// --- Utility Functions ---

// This function checks if the ship is blocked in the given direction and, if so, tries to find an alternative direction to move towards the goal.
hlt::Direction ShipAI::UnblockShip(const std::unique_ptr<hlt::GameMap>& gameMap, const hlt::Direction direction, const hlt::Position& goal){
	if (direction == hlt::Direction::STILL) {
		if (gameMap->at({ goal.x + 1, goal.y })->is_empty()) {
			return hlt::Direction::EAST;
		}

		if (gameMap->at({ goal.x - 1, goal.y })->is_empty()) {
			return hlt::Direction::WEST;
		}

		if (gameMap->at({ goal.x, goal.y + 1 })->is_empty()) {
			return hlt::Direction::SOUTH;
		}

		if (gameMap->at({ goal.x, goal.y - 1 })->is_empty()) {
			return hlt::Direction::NORTH;
		}
	}

	return direction;
}