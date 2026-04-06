#include <random>

#include "include/shipyard_ai.hpp"
#include "include/ship_ai.hpp"

int main(int argc, char* argv[]) {
	hlt::Game game;
    game.ready("PoissonSteve");

	// Get the random seed from command line arguments or use the current time if not provided.
	unsigned int rngSeed;
	if (argc > 1) rngSeed = (unsigned int) std::stoul(argv[1]);
	else rngSeed = (unsigned int) time(nullptr);

	// Get the behavior tree for shipyard AI.
	BehaviorTree::Selector<ShipyardPayload> shipyardAi = ShipyardAI::GetBehaviorTree();

	// Get the behavior trees for miner and explorer ship AIs.
    BehaviorTree::Selector<ShipPayload> minerAi = ShipAI::GetMinerBehaviorTree();
	BehaviorTree::Selector<ShipPayload> explorerAi = ShipAI::GetExplorerBehaviorTree();

	// Create the different Managers for handling units, spots and moves.
	UnitManager unitManager(rngSeed);
    SpotManager spotManager;
    MoveManager moveManager;

	// Main game loop
    while (true) {
		// Update the game state for the new turn.
        game.update_frame();

		alreadyDropoff = false; // Reset the flag for building dropoffs at the start of each turn.
		spotManager.ResetObjectives(); // Reset the targeted spots at the start of each turn.

		// Get references to the current player and game map for easier access.
        const std::shared_ptr<hlt::Player> me = game.me;
        std::unique_ptr<hlt::GameMap>& gameMap = game.game_map;
        std::vector<hlt::Command> commandQueue;

		// Evaluate the behavior tree for each ship and populate the command queue.
        for (const auto& shipIterator : me->ships) {
	        const std::shared_ptr<hlt::Ship> ship = shipIterator.second;
			const ShipPayload payload{ game, commandQueue, ship, spotManager, moveManager };

			// TryAdd and Choice the unit type for the ship and evaluate the corresponding behavior tree.
            unitManager.TryAddUnit(ship->id);
	        switch (unitManager.GetUnitType(ship->id)) {
	        case UnitType::Miner:
				minerAi.Evaluate(payload);
		        break;
	        case UnitType::Explorer:
				explorerAi.Evaluate(payload);
		        break;
	        }
        }

		// Evaluate the behavior tree for the shipyard and populate the command queue.
        shipyardAi.Evaluate({game,commandQueue,me->shipyard});

		// Send the command queue to the game engine and end the turn.
        if (!game.end_turn(commandQueue)) {
            break;
        }
    }

    return 0;
}
