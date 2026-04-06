#pragma once

#include <queue>

#include "hlt/game_map.hpp"
#include "hlt/position.hpp"
#include "hlt/ship.hpp"

// Manages the movement of ships using A* pathfinding algorithm.
class MoveManager {
public:
	// Returns the next direction for the ship to move towards the destination using A* pathfinding.
	hlt::Direction GetNextDirection(const std::unique_ptr<hlt::GameMap>& gameMap, std::shared_ptr<hlt::Ship>& ship, const hlt::Position& destination);
	
	// Returns the total cost of the path from the ship's current position to the destination using A* pathfinding.
	int GetCostPath(const std::unique_ptr<hlt::GameMap>& gameMap, std::shared_ptr<hlt::Ship>& ship, const hlt::Position& destination);
private:
	// Data structure to hold the state of the A* pathfinding algorithm.
	struct PathState {
		std::vector<int> gScore;
		std::vector<int> parent;
		std::vector<bool> closed;
		std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> open;
	};

	// Runs the A* pathfinding algorithm and updates the PathState with the results.
	static bool RunAStar(const std::unique_ptr<hlt::GameMap>& gameMap, const hlt::Position& startPos, const hlt::Position& destPos, PathState& state);

	// Helper functions for the A* pathfinding algorithm.
	static int GetFirstStepIndex(const PathState& state, int startIdx, int destIdx);
	static int GetIndexFromPosition(const hlt::Position& position, int width);
	static hlt::Position GetPositionFromIndex(int index, int width);
	static hlt::Direction GetComputeDirection(const hlt::Position& startPos, const hlt::Position& stepPos, const hlt::Position& size);
};
