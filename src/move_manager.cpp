#include "include/move_manager.hpp"

hlt::Direction MoveManager::GetNextDirection(const std::unique_ptr<hlt::GameMap>& gameMap, std::shared_ptr<hlt::Ship>& ship, const hlt::Position& destination) {
	return gameMap->naive_navigate(ship, destination);
}
