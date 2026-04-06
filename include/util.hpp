#pragma once

#include "hlt/game.hpp"
#include "hlt/constants.hpp"
#include "hlt/log.hpp"

// -- SHIP --

// Constants for the ship behavior tree nodes.
constexpr double DISTANCE_PENALTY = 8.0;

// Contains data passed to the ship behavior tree nodes.
struct ShipPayload {
    hlt::Game& game;                     // Reference to the current game state and logic.
    std::vector<hlt::Command>& commands; // Reference to the list of pending commands.
    std::shared_ptr<hlt::Ship> ship;     // Pointer to the ship associated with the node.
};

// -- SHIPYARD --

// Constants for the shipyard behavior tree nodes.
constexpr int BONUS_COST_SHIP = 500;
constexpr int MAX_SHIP = 13;

// Contains data passed to the shipyard behavior tree nodes.
struct ShipyardPayload {
    hlt::Game& game;                         // Reference to the current game state and logic.
    std::vector<hlt::Command>& commands;     // Reference to the list of pending commands.
    std::shared_ptr<hlt::Shipyard> shipyard; // Pointer to the shipyard associated with the node.
};