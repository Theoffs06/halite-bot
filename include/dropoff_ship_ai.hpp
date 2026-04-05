#pragma once

#include "include/util.hpp"
#include "include/behavior_tree.hpp"

namespace DropoffShipAi {

class TransformIntoDropoff : public BehaviorTree::Leaf<Payload> {
public:
    TransformIntoDropoff(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            unsigned int availableHalite = payload.ship->halite + payload.game.me->halite + payload.game.game_map->at(payload.ship->position)->halite;
            if (availableHalite >= 4000) {
                payload.commands.push_back(payload.ship->make_dropoff());
                return BehaviorTree::NodeState::Success;
            }

            return BehaviorTree::NodeState::Failure;
        };
    }
};

class MoveToBigHaliteSpot : public BehaviorTree::Leaf<Payload> {
public:
    MoveToBigHaliteSpot(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            hlt::Position bestPositionSoFar;
            unsigned int maxHaliteSoFar = 0;
            unsigned int currentDistance = 100000000;

            for (int y = 0; y < payload.game.game_map->height; y++) {
                for (int x = 0; x < payload.game.game_map->width; x++) {
                    hlt::MapCell* cell = payload.game.game_map->at({ x, y });
                    if (cell->halite > maxHaliteSoFar && !cell->has_structure()) {
                        bestPositionSoFar = cell->position;
                        maxHaliteSoFar = cell->halite;
                    }
                    else if (cell->halite == maxHaliteSoFar) {
                        int distance = payload.game.game_map->calculate_distance(payload.ship->position, { x ,y });
                        if (distance < currentDistance) {
                            currentDistance = distance;
                            bestPositionSoFar = cell->position;
                        }
                    }
                }
            }

            hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, bestPositionSoFar);
            payload.commands.push_back(payload.ship->move(direction));

            return BehaviorTree::NodeState::Running;
        };
    }
};

class IsOnBigHaliteSpot : public BehaviorTree::Leaf<Payload> {
public:
    IsOnBigHaliteSpot(BehaviorTree::Node<Payload>* parent)
        : Leaf(parent)
    {
        this->evaluation = [&](Payload payload) {
            unsigned int totalHalite = 0;
            hlt::Position pos = payload.ship->position;
            for (int y = pos.y - 2; y < pos.y + 2; y++) {
                for (int x = pos.x - 2; x < pos.x + 2; x++) {
                    totalHalite += payload.game.game_map->at({ x, y })->halite;
                }
            }

            if (payload.game.game_map->at(payload.ship->position)->halite >= 800) {
                return BehaviorTree::NodeState::Success;
            }

            return BehaviorTree::NodeState::Failure;
        };
    }
};

BehaviorTree::Selector<Payload> GetBehaviorTree(BehaviorTree::Node<Payload>* parent = nullptr) {
    static BehaviorTree::Selector<Payload> root(parent);

    static BehaviorTree::Sequencer<Payload> shouldTransformIntoDropoff(&root);
    static IsOnBigHaliteSpot isOnBigHaliteSpot(&shouldTransformIntoDropoff);
    static TransformIntoDropoff transformIntoDropoff(&shouldTransformIntoDropoff);

    // static MoveToBigHaliteSpot MoveToBigHaliteSpot(&root);

    return root;
}

}
