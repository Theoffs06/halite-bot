#pragma once

#include <unordered_set>

#include "hlt/game_map.hpp"
#include "hlt/position.hpp"
#include "hlt/ship.hpp"

// Manages the targeting of halite spots for ships.
class SpotManager {
public:
	// Reset the set of currently targeted spots.
	void ResetObjectives();

	// Get the best halite spot for a given ship, ensuring that it is not already targeted by another ship.
	hlt::Position GetTheBestHaliteSpot(const std::unique_ptr<hlt::GameMap>& gameMap, const std::shared_ptr<hlt::Ship>& ship);
private:
	std::unordered_set<unsigned int> m_currentlyTargetedSpots;
};
