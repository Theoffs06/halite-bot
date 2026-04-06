#pragma once

#include "include/util.hpp"
#include "include/dropoff_ship_ai.hpp"

namespace ShipAI {
	// --- Behavior Tree Construction ---

	BehaviorTree::Selector<ShipPayload> GetMinerBehaviorTree();
	BehaviorTree::Selector<ShipPayload> GetExplorerBehaviorTree();

	// --- Behavior Tree Nodes ---

	// Leaf: Checks if the game is about to end or if the ship is on a good spot to drop off halite, and should deposit if either of those conditions are true.
	class EndShouldDeposit : public BehaviorTree::Leaf<ShipPayload> {
	public:
		EndShouldDeposit(BehaviorTree::Node<ShipPayload>* parent);
	};

	// Leaf: Checks if the ship should deposit halite (going back to the shipyard to drop off halite).
	class ShouldDeposit : public BehaviorTree::Leaf<ShipPayload> {
	public:
		ShouldDeposit(BehaviorTree::Node<ShipPayload>* parent);
	};

	// Leaf: Commands the ship to go back to the shipyard to drop off halite.
	class GoDeposit : public BehaviorTree::Leaf<ShipPayload> {
	public:
		GoDeposit(BehaviorTree::Node<ShipPayload>* parent);
	};

	// Leaf: Checks if the ship is currently on a halite spot with halite to collect.
	class HaliteHere : public BehaviorTree::Leaf<ShipPayload> {
	public:
		HaliteHere(BehaviorTree::Node<ShipPayload>* parent);
	};

	// Leaf: Checks if the ship is not full and can continue collecting halite.
	class IsNotFull : public BehaviorTree::Leaf<ShipPayload> {
	public:
		IsNotFull(BehaviorTree::Node<ShipPayload>* parent);
	};

	// Leaf: Commands the ship to collect halite from the current cell.
	class CollectHalite : public BehaviorTree::Leaf<ShipPayload> {
	public:
		CollectHalite(BehaviorTree::Node<ShipPayload>* parent);
	};

	// Leaf: Commands the ship to move to the best nearby halite spot to mine.
	class MoveToBestHaliteSpot : public BehaviorTree::Leaf<ShipPayload> {
	public:
		MoveToBestHaliteSpot(BehaviorTree::Node<ShipPayload>* parent);
	};

	// --- Utility functions ---

	// Get the nearest dropoff position (either the shipyard or an existing dropoff) for the given ship.
	hlt::Position GetNearestDropoffPosition(const std::unique_ptr<hlt::GameMap>& gameMap, std::shared_ptr<hlt::Player> me, const std::shared_ptr<hlt::Ship>& ship, const std::shared_ptr<hlt::Shipyard>& shipyard);
} // namespace ShipAI
