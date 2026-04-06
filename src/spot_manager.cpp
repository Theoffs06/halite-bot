#include "include/spot_manager.hpp"
#include "include/util.hpp"

void SpotManager::ResetObjectives() {
	m_currentlyTargetedSpots.clear();
}

hlt::Position SpotManager::GetTheBestHaliteSpot(const std::unique_ptr<hlt::GameMap>& gameMap, const std::shared_ptr<hlt::Ship>& ship) {
	const unsigned int width = gameMap->width;
	const unsigned int height = gameMap->height;

	hlt::Position bestSpotPosition = ship->position;
	double bestSpotScore = -std::numeric_limits<double>::infinity();
	unsigned int bestSpotDistance = std::numeric_limits<unsigned int>::max();

	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			const auto& cell = gameMap->at({
				(int) x,
				(int) y
			});

			if (cell->has_structure() || cell->is_occupied()) continue;

			const unsigned int distance = gameMap->calculate_distance(ship->position, cell->position);
			const bool alreadyTargeted = m_currentlyTargetedSpots.contains(y * width + x);

			const double score = cell->halite - (distance * DISTANCE_PENALTY) - (alreadyTargeted ? TARGETED_PENALTY : 0.0);
			if (score > bestSpotScore || (score == bestSpotScore && distance < bestSpotDistance)) {
				bestSpotScore = score;
				bestSpotPosition = cell->position;
				bestSpotDistance = distance;
			}
		}
	}

	m_currentlyTargetedSpots.insert((unsigned int) bestSpotPosition.y * width + (unsigned int) bestSpotPosition.x);
	return bestSpotPosition;
}