#pragma once

#include "include/util.hpp"
#include "include/dropoff_ship_ai.hpp"
#include "include/behavior_tree.hpp"

namespace ShipAI {

class HaliteHere : public BehaviorTree::Leaf<Payload> {
public:
    HaliteHere(BehaviorTree::Node<Payload>* parent);
};

class IsNotFull : public BehaviorTree::Leaf<Payload> {
public:
    IsNotFull(BehaviorTree::Node<Payload>* parent);
};

class ShouldDeposit : public BehaviorTree::Leaf<Payload> {
public:
    ShouldDeposit(BehaviorTree::Node<Payload>* parent);
};

class GoDeposit : public BehaviorTree::Leaf<Payload> {
public:
    GoDeposit(BehaviorTree::Node<Payload>* parent);
};

class MoveToCoolHaliteSpot : public BehaviorTree::Leaf<Payload> {
public:
    MoveToCoolHaliteSpot(BehaviorTree::Node<Payload>* parent);
};

class CollectHalite : public BehaviorTree::Leaf<Payload> {
public:
    CollectHalite(BehaviorTree::Node<Payload>* parent);
};

inline BehaviorTree::Selector<Payload> GetBehaviorTree() {
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
