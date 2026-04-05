#pragma once

#include "include/util.hpp"
#include "include/dropoff_ship_ai.hpp"
#include "include/behavior_tree.hpp"

namespace ShipAI {
	// --- Leaf Nodes ---

	// Checks if there is halite at the current ship's position.
	class HaliteHere : public BehaviorTree::Leaf<Payload> {
	public:
	    HaliteHere(BehaviorTree::Node<Payload>* parent);
	};

	// Checks if the ship's cargo is not full.
	class IsNotFull : public BehaviorTree::Leaf<Payload> {
	public:
	    IsNotFull(BehaviorTree::Node<Payload>* parent);
	};

	// Checks if the ship should deposit halite at a dropoff point.
	class ShouldDeposit : public BehaviorTree::Leaf<Payload> {
	public:
	    ShouldDeposit(BehaviorTree::Node<Payload>* parent);
	};

	// Moves the ship to a dropoff point to deposit halite.
	class GoDeposit : public BehaviorTree::Leaf<Payload> {
	public:
	    GoDeposit(BehaviorTree::Node<Payload>* parent);
	};

	// Moves the ship to a spot with "cool" (low-risk) halite.
	class MoveToCoolHaliteSpot : public BehaviorTree::Leaf<Payload> {
	public:
	    MoveToCoolHaliteSpot(BehaviorTree::Node<Payload>* parent);
	};

	// Collects halite at the current position.
	class CollectHalite : public BehaviorTree::Leaf<Payload> {
	public:
	    CollectHalite(BehaviorTree::Node<Payload>* parent);
	};

	// --- Behavior Tree Construction ---

	// Builds and returns the behavior tree for a ship AI.
	// The tree is static to avoid rebuilding it every time the function is called.
	BehaviorTree::Selector<Payload> GetBehaviorTree();
} // namespace ShipAI