#pragma once

#include "hlt/game.hpp"
#include "hlt/constants.hpp"
#include "hlt/log.hpp"

#include "BehaviorTree.h"

#include <random>

namespace ShipAI {

unsigned int rng_seed = static_cast<unsigned int>(time(nullptr));
std::mt19937 rng(rng_seed);

struct Payload {
    hlt::Game& game;
    std::vector<hlt::Command>& commands;
    std::shared_ptr<hlt::Ship> ship;
};

class HaliteHere : public BehaviorTree::Leaf<Payload> {
public:
    HaliteHere(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            if (payload.game.game_map->at(payload.ship)->halite > 50) {
                return BehaviorTree::NodeState::Success;
            }

            return BehaviorTree::NodeState::Failure;
        };
    }
};

class IsNotFull : public BehaviorTree::Leaf<Payload> {
public:
    IsNotFull(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            if (payload.ship->is_full()) {
                return BehaviorTree::NodeState::Failure;
            }

            return BehaviorTree::NodeState::Success;
        };
    }
};

class ShouldDeposit : public BehaviorTree::Leaf<Payload> {
public:
    ShouldDeposit(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            unsigned int distanceToBase = payload.game.game_map->calculate_distance(payload.ship->position, payload.game.me->shipyard->position);
            auto moves = payload.game.game_map->get_unsafe_moves(payload.ship->position, payload.game.me->shipyard->position);

            unsigned int cost = 0;
            hlt::Position currentPos = payload.ship->position;
            for (hlt::Direction move : moves) {
                switch (move) {
                case hlt::Direction::NORTH:
                    currentPos.y--;
                    cost += payload.game.game_map->at(currentPos)->halite / 10;
                    break;
                case hlt::Direction::SOUTH:
                    currentPos.y++;
                    cost += payload.game.game_map->at(currentPos)->halite / 10;
                    break;
                case hlt::Direction::WEST:
                    currentPos.x--;
                    cost += payload.game.game_map->at(currentPos)->halite / 10;
                    break;
                case hlt::Direction::EAST:
                    currentPos.x++;
                    cost += payload.game.game_map->at(currentPos)->halite / 10;
                    break;
                default:
                    break;
                }
            }

            if (payload.ship->halite >= cost + 800) {
                return BehaviorTree::NodeState::Success;
            }

            return BehaviorTree::NodeState::Failure;
        };
    }
};

class GoHome : public BehaviorTree::Leaf<Payload> {
public:
    GoHome(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, payload.game.me->shipyard->position);
            payload.commands.push_back(payload.ship->move(direction));
            return BehaviorTree::NodeState::Running;
        };
    }
};

class MoveToCoolHaliteSpot : public BehaviorTree::Leaf<Payload> {
public:
    MoveToCoolHaliteSpot(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            
            hlt::Position bestPositionSoFar;
            unsigned int maxHaliteSoFar = 0;

            hlt::Position pos = payload.ship->position;
            for (unsigned int y = pos.y - 2; y < pos.y + 2; y++) {
                for (unsigned int x = pos.x - 2; x < pos.x + 2; x++) {
                    hlt::MapCell* cell = payload.game.game_map->at({ (int)x, (int)y });
                    if (cell->halite > maxHaliteSoFar && !cell->is_occupied()) {
                        bestPositionSoFar = cell->position;
                        maxHaliteSoFar = cell->halite;
                    }
                }
            }

            hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, bestPositionSoFar);

            /*
            unsigned int numTries = 0;
            while (payload.game.game_map->at(payload.ship->position.directional_offset(direction))->is_occupied()) {
                if (numTries >= 4) {
                    direction = hlt::Direction::STILL;
                    break;
                }

                if (direction == hlt::Direction::NORTH) {
                    direction = hlt::Direction::EAST;
                }
                else if (direction == hlt::Direction::EAST) {
                    direction = hlt::Direction::SOUTH;
                }
                else if (direction == hlt::Direction::SOUTH) {
                    direction = hlt::Direction::WEST;
                }
                else if (direction == hlt::Direction::WEST) {
                    direction = hlt::Direction::NORTH;
                }

                numTries++;
            }
            */
            payload.commands.push_back(payload.ship->move(direction));

            return BehaviorTree::NodeState::Running;
        };
    }
};

class CollectHalite : public BehaviorTree::Leaf<Payload> {
public:
    CollectHalite(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            payload.commands.push_back(payload.ship->stay_still());
            return BehaviorTree::NodeState::Running;
        };
    }
};

BehaviorTree::Selector<Payload> GetBehaviorTree() {
    static BehaviorTree::Selector<ShipAI::Payload> root(nullptr);

    static BehaviorTree::Sequencer<ShipAI::Payload> goHomeIfEnoughHalite(&root);
    static ShouldDeposit shouldDeposit(&goHomeIfEnoughHalite);
    static GoHome goHome(&goHomeIfEnoughHalite);

    static BehaviorTree::Sequencer<ShipAI::Payload> mining(&root);
    static IsNotFull isNotFull(&mining);
    static HaliteHere haliteHere(&mining);
    static CollectHalite collectHalite(&mining);

    static MoveToCoolHaliteSpot moveToCoolHaliteSpot(&root);

    return root;
}

}
