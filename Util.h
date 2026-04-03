#pragma once

#include "hlt/game.hpp"

struct Payload {
    hlt::Game& game;
    std::vector<hlt::Command>& commands;
    std::shared_ptr<hlt::Ship> ship;
};
