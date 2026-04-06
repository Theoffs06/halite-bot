#pragma once

#include "game_map.hpp"
#include "position.hpp"
#include "ship.hpp"

class MoveManager {
public:
	hlt::Direction GetNextDirection(const std::unique_ptr<hlt::GameMap>& gameMap, std::shared_ptr<hlt::Ship>& ship, const hlt::Position& destination);
};
