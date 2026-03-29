#include "hlt/game.hpp"
#include "hlt/constants.hpp"
#include "hlt/log.hpp"

#include "BehaviorTree.h"
#include "ShipAI.h"

using namespace std;
using namespace hlt;

int main(int argc, char* argv[]) {
    Game game;
    game.ready("TotoEnBot");

    BehaviorTree::Selector<ShipAI::Payload> shipAi = ShipAI::GetBehaviorTree();

    for (;;) {
        game.update_frame();
        shared_ptr<Player> me = game.me;
        unique_ptr<GameMap>& game_map = game.game_map;

        vector<Command> command_queue;

        for (const auto& ship_iterator : me->ships) {
            shared_ptr<Ship> ship = ship_iterator.second;
            shipAi.Evaluate({ game, command_queue, ship });
        }

        if (
            game.turn_number <= 200 &&
            me->halite >= constants::SHIP_COST &&
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
