#pragma once

#include "hlt/game.hpp"
#include "hlt/constants.hpp"
#include "hlt/log.hpp"

struct Payload {
    hlt::Game& game;
    std::vector<hlt::Command>& commands;
    std::shared_ptr<hlt::Ship> ship;
};
