#pragma once

#include <queue>

#include "game_map.hpp"
#include "position.hpp"
#include "ship.hpp"

class MoveManager {
public:
	hlt::Direction GetNextDirection(const std::unique_ptr<hlt::GameMap>& gameMap, std::shared_ptr<hlt::Ship>& ship, const hlt::Position& destination);
private:
	struct PathState {
		std::vector<int> gScore;
		std::vector<int> parent;
		std::vector<bool> closed;
		std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> open;
	};

	int GetFirstStepIndex(const PathState& state, int startIdx, int destIdx);
	int GetIndexFromPosition(const hlt::Position& position, int width);
	hlt::Position GetPositionFromIndex(int index, int width);
	hlt::Direction GetComputeDirection(const hlt::Position& startPos, const hlt::Position& stepPos, const hlt::Position& size);
};
