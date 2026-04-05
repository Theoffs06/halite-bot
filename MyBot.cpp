#include "include/ship_ai.hpp"
#include "include/dropoff_ship_ai.hpp"

int main(int argc, char* argv[]) {
    bool firstStep = true;

    hlt::Game game;
    game.ready("TotoEnBot");

    BehaviorTree::Selector<Payload> shipAi = ShipAI::GetBehaviorTree();
    std::shared_ptr<hlt::Ship> dropoffShip;

    for (;;) {
        game.update_frame();
        std::shared_ptr<hlt::Player> me = game.me;
        std::unique_ptr<hlt::GameMap>& game_map = game.game_map;

        std::vector<hlt::Command> command_queue;

        for (const auto& ship_iterator : me->ships) {
	        std::shared_ptr<hlt::Ship> ship = ship_iterator.second;
            shipAi.Evaluate({ game, command_queue, ship });
        }

        if (
            game.turn_number <= 200 &&
            me->halite >= hlt::constants::SHIP_COST * me->ships.size() &&
            !game_map->at(me->shipyard)->is_occupied())
        {
            command_queue.push_back(me->shipyard->spawn());
        }

        if (!game.end_turn(command_queue)) {
            break;
        }
    }

    return 0;
}
