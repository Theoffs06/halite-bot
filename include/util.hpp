#pragma once

#include "spot_manager.hpp"
#include "hlt/game.hpp"
#include "hlt/constants.hpp"
#include "hlt/log.hpp"

// -- SHIP --

// Constants for the ship behavior tree nodes.
constexpr int END_TURN = 350;
constexpr int INTERESTING_HALITE_CELL = 10;
constexpr int END_GO_HOME_HALITE = 100;
constexpr int GO_HOME_HALITE = 500;
constexpr double TARGETED_PENALTY = 900.0;
constexpr double DISTANCE_PENALTY = 1.5;

// Contains data passed to the ship behavior tree nodes.
struct ShipPayload {
    hlt::Game& game;                     // Reference to the current game state and logic.
    std::vector<hlt::Command>& commands; // Reference to the list of pending commands.
    std::shared_ptr<hlt::Ship> ship;     // Pointer to the ship associated with the node.

    SpotManager& spotManager;            // Reference of the SpotManager for finding optimal halite spots.
};

// -- SHIPYARD --

// Constants for the shipyard behavior tree nodes.
constexpr int BONUS_COST_SHIP = 0;
constexpr int MAX_SHIP = 13;

// Contains data passed to the shipyard behavior tree nodes.
struct ShipyardPayload {
    hlt::Game& game;                         // Reference to the current game state and logic.
    std::vector<hlt::Command>& commands;     // Reference to the list of pending commands.
    std::shared_ptr<hlt::Shipyard> shipyard; // Pointer to the shipyard associated with the node.
};