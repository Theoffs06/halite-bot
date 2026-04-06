#include "include/ship_ai.hpp"

BehaviorTree::Selector<Payload> ShipAI::GetBehaviorTree() {
	// Root node: selects between mining or depositing
	static BehaviorTree::Selector<Payload> root(nullptr);
	
	return root;
}
