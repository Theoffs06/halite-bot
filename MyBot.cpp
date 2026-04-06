#include <random>

#include "unit_manager.hpp"
#include "include/shipyard_ai.hpp"
#include "include/ship_ai.hpp"

int main(int argc, char* argv[]) {
    unsigned int rngSeed;
    if (argc > 1) rngSeed = (unsigned int) std::stoul(argv[1]);
    else rngSeed = (unsigned int) time(nullptr);

	hlt::Game game;
    game.ready("PoissonSteve");

	// Get the behavior tree for shipyard AI.
	BehaviorTree::Selector<ShipyardPayload> shipyardAi = ShipyardAI::GetBehaviorTree();

	// Get the behavior tree for ship AI.
    BehaviorTree::Selector<ShipPayload> minerAi = ShipAI::GetMinerBehaviorTree();
	BehaviorTree::Selector<ShipPayload> explorerAi = ShipAI::GetExplorerBehaviorTree();

	UnitManager unitManager(rngSeed);
    SpotManager spotManager;
    MoveManager moveManager;

	// Main game loop
    while (true) {
		// Update the game state for the new turn.
        game.update_frame();

        spotManager.ResetObjectives();

		// Get references to the current player and game map for easier access.
        const std::shared_ptr<hlt::Player> me = game.me;
        std::unique_ptr<hlt::GameMap>& gameMap = game.game_map;
        std::vector<hlt::Command> commandQueue;

		// Evaluate the behavior tree for each ship and populate the command queue.
        for (const auto& shipIterator : me->ships) {
	        const std::shared_ptr<hlt::Ship> ship = shipIterator.second;
			const ShipPayload payload{ game, commandQueue, ship, spotManager, moveManager };

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
