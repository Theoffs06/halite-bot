#pragma once

#include <random>

#include "include/util.hpp"
#include "include/dropoff_ship_ai.hpp"
#include "include/ship_ai.hpp"

namespace ShipAI {

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
            /*
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
            */

            if (payload.ship->halite >= cost + 800) {
                return BehaviorTree::NodeState::Success;
            }

            return BehaviorTree::NodeState::Failure;
        };
    }
};

class GoDeposit : public BehaviorTree::Leaf<Payload> {
public:
    GoDeposit(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            unsigned int bestDistanceSoFar = 1000000000;
            hlt::Position closestDropoffSoFar;
            for (auto dropoff : payload.game.me->dropoffs) {
                unsigned int distance = payload.game.game_map->calculate_distance(dropoff.second->position, payload.ship->position);
                if (distance < bestDistanceSoFar) {
                    bestDistanceSoFar = distance;
                    closestDropoffSoFar = dropoff.second->position;
                }
            }

            if (payload.game.game_map->calculate_distance(payload.game.me->shipyard->position, payload.ship->position) < bestDistanceSoFar) {
                closestDropoffSoFar = payload.game.me->shipyard->position;
            }

            hlt::Position shipPos = payload.ship->position;
            hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, closestDropoffSoFar);
            if (direction == hlt::Direction::STILL) {
                if (payload.game.game_map->at({ shipPos.x + 1, shipPos.y })->is_empty()) {
                    direction = hlt::Direction::EAST;
                }
                else if (payload.game.game_map->at({ shipPos.x - 1, shipPos.y })->is_empty()) {
                    direction = hlt::Direction::WEST;
                }
                else if (payload.game.game_map->at({ shipPos.x, shipPos.y + 1 })->is_empty()) {
                    direction = hlt::Direction::SOUTH;
                }
                else if (payload.game.game_map->at({ shipPos.x, shipPos.y - 1 })->is_empty()) {
                    direction = hlt::Direction::NORTH;
                }
            }
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
            for (unsigned int y = pos.y - 8; y < pos.y + 5; y++) {
                for (unsigned int x = pos.x - 5; x < pos.x + 5; x++) {
                    hlt::MapCell* cell = payload.game.game_map->at({ (int)x, (int)y });
                    if (cell->halite > maxHaliteSoFar && cell->is_empty()) {
                        bestPositionSoFar = cell->position;
                        maxHaliteSoFar = cell->halite;
                    }
                }
            }

            hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, bestPositionSoFar);
            if (direction == hlt::Direction::STILL) {
                if (payload.game.game_map->at({ bestPositionSoFar.x + 1, bestPositionSoFar.y })->is_empty()) {
                    direction = hlt::Direction::EAST;
                }
                else if (payload.game.game_map->at({ bestPositionSoFar.x - 1, bestPositionSoFar.y })->is_empty()) {
                    direction = hlt::Direction::WEST;
                }
                else if (payload.game.game_map->at({ bestPositionSoFar.x, bestPositionSoFar.y + 1 })->is_empty()) {
                    direction = hlt::Direction::SOUTH;
                }
                else if (payload.game.game_map->at({ bestPositionSoFar.x, bestPositionSoFar.y - 1 })->is_empty()) {
                    direction = hlt::Direction::NORTH;
                }
            }
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
    static BehaviorTree::Selector<Payload> root(nullptr);

    static BehaviorTree::Selector<Payload> dropoffShipAi = DropoffShipAi::GetBehaviorTree(&root);

    static BehaviorTree::Sequencer<Payload> goDepositIfEnoughHalite(&root);
    static ShouldDeposit shouldDeposit(&goDepositIfEnoughHalite);
    static GoDeposit goDeposit(&goDepositIfEnoughHalite);

    static BehaviorTree::Sequencer<Payload> mining(&root);
    static IsNotFull isNotFull(&mining);
    static HaliteHere haliteHere(&mining);
    static CollectHalite collectHalite(&mining);

    static MoveToCoolHaliteSpot moveToCoolHaliteSpot(&root);

    return root;
}

}
