#pragma once

#include <unordered_set>

#include "game_map.hpp"
#include "position.hpp"
#include "ship.hpp"

class SpotManager {
public:
	void ResetObjectives();

	hlt::Position GetTheBestHaliteSpot(const std::unique_ptr<hlt::GameMap>& gameMap, const std::shared_ptr<hlt::Ship>& ship);
private:
	std::unordered_set<unsigned int> m_currentlyTargetedSpots;
};
