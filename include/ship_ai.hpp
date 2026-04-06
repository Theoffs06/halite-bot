#pragma once

#include "include/util.hpp"
#include "include/behavior_tree.hpp"

namespace ShipAI {
	// Constructs and returns the behavior tree for ship AI.
	// The tree is static to avoid rebuilding it every time the function is called.
	BehaviorTree::Selector<Payload> GetBehaviorTree();
} // namespace ShipAI
