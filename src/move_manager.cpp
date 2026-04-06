#include "include/move_manager.hpp"

#include <queue>

hlt::Direction MoveManager::GetNextDirection(const std::unique_ptr<hlt::GameMap>& gameMap, std::shared_ptr<hlt::Ship>& ship, const hlt::Position& destination) {
	const int width = gameMap->width;
    const int height = gameMap->height;
    const auto normalizePos = [&](const hlt::Position& p) { return gameMap->normalize(p); };

    const hlt::Position startPos = normalizePos(ship->position);
    const hlt::Position destPos = normalizePos(destination);

    if (startPos == destPos) return hlt::Direction::STILL;

    const auto idxFromPos = [&](const hlt::Position& p) {
        return p.x + p.y * width;
    };

    const auto posFromIdx = [&](int idx) {
        return hlt::Position{ idx % width, idx / width };
    };

    const int totalCells = width * height;
    std::vector<int> gScore(totalCells, INT_MAX);
    std::vector<int> parent(totalCells, -1);
    std::vector<char> closed(totalCells, 0);

    const int startIdx = idxFromPos(startPos);
    const int destIdx = idxFromPos(destPos);

    // min-heap of (fScore, index)
    using PQItem = std::pair<int, int>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> open;

    gScore[startIdx] = 0;
    const int startH = gameMap->calculate_distance(startPos, destPos);
    open.emplace(startH, startIdx);

    while (!open.empty()) {
        const auto [currentF, currentIdx] = open.top();
        open.pop();

        if (closed[currentIdx]) continue;
        closed[currentIdx] = 1;

        if (currentIdx == destIdx) {
            // Reconstruct path: step forward from start to the child leading towards dest
            int node = destIdx;
            
        	// Walk until the parent is the start; node will be the first step from start
            while (parent[node] != -1 && parent[node] != startIdx) {
                node = parent[node];
            }

            if (parent[node] == startIdx) {
                const hlt::Position stepPos = posFromIdx(node);
            	if (gameMap->at(stepPos)->is_occupied()) return hlt::Direction::STILL;

                // compute wrapped deltas
                int dx = stepPos.x - startPos.x;
                int dy = stepPos.y - startPos.y;

                // adjust dx for toroidal wrap
                if (dx > width / 2) dx -= width;
                if (dx < -width / 2) dx += width;
                // adjust dy for toroidal wrap
                if (dy > height / 2) dy -= height;
                if (dy < -height / 2) dy += height;

                if (dx == 1) return hlt::Direction::EAST;
                if (dx == -1) return hlt::Direction::WEST;
                if (dy == 1) return hlt::Direction::SOUTH;
                if (dy == -1) return hlt::Direction::NORTH;
            }

            // If we couldn't find a first step (shouldn't happen), return STILL
            return hlt::Direction::STILL;
        }

        const hlt::Position currentPos = posFromIdx(currentIdx);

        for (const auto& direction : hlt::ALL_CARDINALS) {
            hlt::Position neighborRaw = currentPos.directional_offset(direction);
            hlt::Position neighbor = normalizePos(neighborRaw);
            int neighborIdx = idxFromPos(neighbor);

            // Allow stepping onto destination even if it's "occupied"
            if (gameMap->at(neighbor)->is_occupied() && neighborIdx != destIdx) continue;
            if (closed[neighborIdx]) continue;

            const int tentativeG = gScore[currentIdx] + 1;

            if (tentativeG < gScore[neighborIdx]) {
                parent[neighborIdx] = currentIdx;
                gScore[neighborIdx] = tentativeG;
                
            	const int fScore = tentativeG + gameMap->calculate_distance(neighbor, destPos);
                open.emplace(fScore, neighborIdx);
            }
        }
    }

    return hlt::Direction::STILL;
}
