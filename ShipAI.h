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
            if (payload.game.game_map->at(payload.ship)->halite > hlt::constants::MAX_HALITE / 10) {
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
            if (payload.ship->halite >= hlt::constants::MAX_HALITE / 2) {
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

class MoveRandom : public BehaviorTree::Leaf<Payload> {
public:
    MoveRandom(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            hlt::Direction random_direction = hlt::ALL_CARDINALS[rng() % 4];
            payload.commands.push_back(payload.ship->move(random_direction));
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

    static MoveRandom moveRandom(&root);

    return root;
}

}
