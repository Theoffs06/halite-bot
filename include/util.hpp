#pragma once

#include "hlt/game.hpp"
#include "hlt/constants.hpp"
#include "hlt/log.hpp"

// Contains data passed to behavior tree nodes
struct Payload {
    hlt::Game& game;                     // Reference to the current game state and logic.
    std::vector<hlt::Command>& commands; // Reference to the list of pending commands.
    std::shared_ptr<hlt::Ship> ship;     // Pointer to the ship associated with the node.
};