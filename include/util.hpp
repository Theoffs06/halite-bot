#pragma once

#include "hlt/game.hpp"
#include "hlt/constants.hpp"
#include "hlt/log.hpp"

#include "include/behavior_tree.hpp"
#include "include/manager/unit_manager.hpp"
#include "include/manager//spot_manager.hpp"
#include "include/manager//move_manager.hpp"

// -- UNIT MANAGER --
constexpr int MINER_PERCENTAGE = 70;
constexpr int EXPLORER_PERCENTAGE = 10;

// -- SHIP --

// Constants for the ship behavior tree nodes.
constexpr int ENDGAME_START_TURN = 350;
constexpr int MIN_HALITE_TO_MINE = 10;
constexpr int ENDGAME_MIN_HALITE_TO_DEPOSIT = 100;
constexpr int MIN_HALITE_TO_DEPOSIT = 500;
constexpr double TARGETED_PENALTY = 900.0;
constexpr double DISTANCE_PENALTY = 1.5;

constexpr int DROPOFF_COST_OFFSET = 4000;
constexpr int MIN_HALITE_TO_DROPOFF = 400;

// Contains data passed to the ship behavior tree nodes.
struct ShipPayload {
    hlt::Game& game;                     // Reference to the current game state and logic.
    std::vector<hlt::Command>& commands; // Reference to the list of pending commands.
    std::shared_ptr<hlt::Ship> ship;     // Pointer to the ship associated with the node.

    SpotManager& spotManager;            // Reference of the SpotManager for finding optimal halite spots.
	MoveManager& moveManager;            // Reference of the MoveManager for managing ship movements and avoiding collisions.
};

// Globals variables for the ship behavior tree nodes.
static bool alreadyDropoff = false; // Indicates whether a dropoff has already been built during the current turn.

// -- SHIPYARD --

// Constants for the shipyard behavior tree nodes.
constexpr int SHIP_COST_OFFSET = 0;
constexpr int MAX_SHIP = 13;

// Contains data passed to the shipyard behavior tree nodes.
struct ShipyardPayload {
    hlt::Game& game;                         // Reference to the current game state and logic.
    std::vector<hlt::Command>& commands;     // Reference to the list of pending commands.
    std::shared_ptr<hlt::Shipyard> shipyard; // Pointer to the shipyard associated with the node.
};