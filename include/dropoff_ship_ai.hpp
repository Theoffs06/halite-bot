#pragma once

#include "include/util.hpp"
#include "include/behavior_tree.hpp"

namespace DropoffShipAi {

class TransformIntoDropoff : public BehaviorTree::Leaf<Payload> {
public:
    TransformIntoDropoff(BehaviorTree::Node<Payload>* parent);
};

class MoveToBigHaliteSpot : public BehaviorTree::Leaf<Payload> {
public:
    MoveToBigHaliteSpot(BehaviorTree::Node<Payload>* parent);
};

class IsOnBigHaliteSpot : public BehaviorTree::Leaf<Payload> {
public:
    IsOnBigHaliteSpot(BehaviorTree::Node<Payload>* parent);
};

inline BehaviorTree::Selector<Payload> GetBehaviorTree(BehaviorTree::Node<Payload>* parent = nullptr) {
    static BehaviorTree::Selector<Payload> root(parent);

    static BehaviorTree::Sequencer<Payload> shouldTransformIntoDropoff(&root);
    static IsOnBigHaliteSpot isOnBigHaliteSpot(&shouldTransformIntoDropoff);
    static TransformIntoDropoff transformIntoDropoff(&shouldTransformIntoDropoff);

    // static MoveToBigHaliteSpot MoveToBigHaliteSpot(&root);

    return root;
}

}
