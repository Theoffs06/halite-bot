#pragma once

#include "include/util.hpp"
#include "include/behavior_tree.hpp"

namespace DropoffShipAi {
	// --- Leaf Nodes ---

	// Transforms the ship into a dropoff point if conditions are met.
	class TransformIntoDropoff : public BehaviorTree::Leaf<Payload> {
	public:
	    TransformIntoDropoff(BehaviorTree::Node<Payload>* parent);
	};

	// Moves the ship to a spot with a large amount of halite.
	class MoveToBigHaliteSpot : public BehaviorTree::Leaf<Payload> {
	public:
	    MoveToBigHaliteSpot(BehaviorTree::Node<Payload>* parent);
	};

	// Checks if the ship is on a spot with a large amount of halite.
	class IsOnBigHaliteSpot : public BehaviorTree::Leaf<Payload> {
	public:
	    IsOnBigHaliteSpot(BehaviorTree::Node<Payload>* parent);
	};

	// --- Behavior Tree Construction ---

	// Builds and returns the behavior tree for a dropoff ship AI.
	// The tree decides whether to transform the ship into a dropoff point.
	inline BehaviorTree::Selector<Payload> GetBehaviorTree(BehaviorTree::Node<Payload>* parent = nullptr) {
		// Root node: selects between actions
		static BehaviorTree::Selector<Payload> root(parent);

		// Subtree: decide whether to transform into a dropoff point
	    static BehaviorTree::Sequencer<Payload> shouldTransformIntoDropoff(&root);
	    static IsOnBigHaliteSpot isOnBigHaliteSpot(&shouldTransformIntoDropoff);
	    static TransformIntoDropoff transformIntoDropoff(&shouldTransformIntoDropoff);
		
		// (Optional) Subtree: move to a big halite spot
		// static MoveToBigHaliteSpot moveToBigHaliteSpot(&root);
	    
		return root;
	}
} // namespace DropoffShipAi
