#include "include/ship_ai_v2.hpp"

int main(int argc, char* argv[]) {
	hlt::Game game;
    game.ready("PoissonSteve");

    BehaviorTree::Selector<Payload> shipAi = ShipAIV2::GetBehaviorTree();

    while (true) {
        game.update_frame();

        std::shared_ptr<hlt::Player> me = game.me;
        std::unique_ptr<hlt::GameMap>& gameMap = game.game_map;

        std::vector<hlt::Command> commandQueue;

        for (const auto& ship_iterator : me->ships) {
	        std::shared_ptr<hlt::Ship> ship = ship_iterator.second;
            shipAi.Evaluate({ game, commandQueue, ship });
        }

        if (
            me->halite >= hlt::constants::SHIP_COST
            && me->ships.size() < 13 
            && !gameMap->at(me->shipyard)->is_occupied()
            ) {
            commandQueue.push_back(me->shipyard->spawn());
        }

        if (!game.end_turn(commandQueue)) {
            break;
        }
    }

    return 0;
}
