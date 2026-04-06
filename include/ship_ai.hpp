#pragma once

#include "include/util.hpp"

namespace ShipAI {
	// --- Behavior Tree Construction ---

	BehaviorTree::Selector<ShipPayload> GetBehaviorTree();

	// --- Behavior Tree Nodes ---

	class EndShouldDeposit : public BehaviorTree::Leaf<ShipPayload> {
	public:
		EndShouldDeposit(BehaviorTree::Node<ShipPayload>* parent);
	};

	class ShouldDeposit : public BehaviorTree::Leaf<ShipPayload> {
	public:
		ShouldDeposit(BehaviorTree::Node<ShipPayload>* parent);
	};

	class GoDeposit : public BehaviorTree::Leaf<ShipPayload> {
	public:
		GoDeposit(BehaviorTree::Node<ShipPayload>* parent);
	};

	class HaliteHere : public BehaviorTree::Leaf<ShipPayload> {
	public:
		HaliteHere(BehaviorTree::Node<ShipPayload>* parent);
	};

	class IsNotFull : public BehaviorTree::Leaf<ShipPayload> {
	public:
		IsNotFull(BehaviorTree::Node<ShipPayload>* parent);
	};

	class CollectHalite : public BehaviorTree::Leaf<ShipPayload> {
	public:
		CollectHalite(BehaviorTree::Node<ShipPayload>* parent);
	};

	class MoveToBestHaliteSpot : public BehaviorTree::Leaf<ShipPayload> {
	public:
		MoveToBestHaliteSpot(BehaviorTree::Node<ShipPayload>* parent);
	};

	// --- Utility Functions ---

	hlt::Direction UnblockShip(const std::unique_ptr<hlt::GameMap>& gameMap, const hlt::Direction direction, const hlt::Position& shipPos);
} // namespace ShipAI
