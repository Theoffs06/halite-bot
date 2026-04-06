#include "include/move_manager.hpp"

hlt::Direction MoveManager::GetNextDirection(const std::unique_ptr<hlt::GameMap>& gameMap, std::shared_ptr<hlt::Ship>& ship, const hlt::Position& destination) {
	const int width = gameMap->width;
    const int height = gameMap->height;

    const hlt::Position startPos = gameMap->normalize(ship->position);
    const hlt::Position destPos = gameMap->normalize(destination);
    if (startPos == destPos) return hlt::Direction::STILL;

	PathState state{};
    state.gScore.resize(width * height, INT_MAX);
    state.parent.resize(width * height, -1);
    state.closed.resize(width * height, false);

    const int startIndex = GetIndexFromPosition(startPos, width);
    const int destinationIndex = GetIndexFromPosition(destPos, width);

    state.gScore[startIndex] = 0;

    const int startHeuristic = gameMap->calculate_distance(startPos, destPos);
    state.open.emplace(startHeuristic, startIndex);

    while (!state.open.empty()) {
        const auto [currentF, currentIdx] = state.open.top();
        state.open.pop();

        if (state.closed[currentIdx]) continue;
        state.closed[currentIdx] = true;

        if (currentIdx == destinationIndex) {
            const int firstStepIdx = GetFirstStepIndex(state, startIndex, destinationIndex);
            if (firstStepIdx == -1) return hlt::Direction::STILL;

        	const hlt::Position stepPos = GetPositionFromIndex(firstStepIdx, width);
        	if (gameMap->at(stepPos)->is_occupied()) return hlt::Direction::STILL;
        	return GetComputeDirection(startPos, stepPos, { width, height });
        }

        const hlt::Position currentPos = GetPositionFromIndex(currentIdx, width);
        for (const auto& direction : hlt::ALL_CARDINALS) {
            hlt::Position neighborRaw = currentPos.directional_offset(direction);
            hlt::Position neighbor = gameMap->normalize(neighborRaw);
            int neighborIdx = GetIndexFromPosition(neighbor, width);

            if (gameMap->at(neighbor)->is_occupied() && neighborIdx != destinationIndex) continue;
            if (state.closed[neighborIdx]) continue;

            const int tentativeG = state.gScore[currentIdx] + 1;
            if (tentativeG < state.gScore[neighborIdx]) {
                state.parent[neighborIdx] = currentIdx;
                state.gScore[neighborIdx] = tentativeG;
                
            	const int fScore = tentativeG + gameMap->calculate_distance(neighbor, destPos);
                state.open.emplace(fScore, neighborIdx);
            }
        }
    }

    return hlt::Direction::STILL;
}

int MoveManager::GetFirstStepIndex(const PathState& state, int startIdx, int destIdx) {
    int node = destIdx;

    while (state.parent[node] != -1 && state.parent[node] != startIdx) {
        node = state.parent[node];
    }

	return (state.parent[node] == startIdx) ? node : -1;
}

int MoveManager::GetIndexFromPosition(const hlt::Position& position, int width) {
	return position.x + position.y * width;
}

hlt::Position MoveManager::GetPositionFromIndex(int index, int width) {
    return hlt::Position{ index % width, index / width };
}

hlt::Direction MoveManager::GetComputeDirection(const hlt::Position& startPos, const hlt::Position& stepPos, const hlt::Position& size) {
    int dx = stepPos.x - startPos.x;
    int dy = stepPos.y - startPos.y;

    if (dx > size.x / 2) dx -= size.x;
    if (dx < -size.x / 2) dx += size.x;
    if (dy > size.y / 2) dy -= size.y;
    if (dy < -size.y / 2) dy += size.y;

    if (dx == 1) return hlt::Direction::EAST;
    if (dx == -1) return hlt::Direction::WEST;
    if (dy == 1) return hlt::Direction::SOUTH;
    if (dy == -1) return hlt::Direction::NORTH;
	return hlt::Direction::STILL;
}
