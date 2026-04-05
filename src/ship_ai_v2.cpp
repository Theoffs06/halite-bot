#include "include/ship_ai_v2.hpp"

#include <unordered_set>
#include <limits>

static std::unordered_set<unsigned int> returningShips;
static std::unordered_set<unsigned int> targetedCells;
static int lastProcessedTurn = -1;

// --- ShipAIV2::IsFull ---
ShipAIV2::IsFull::IsFull(BehaviorTree::Node<Payload>* parent) : Leaf(parent) {
    this->evaluation = [&](const Payload& payload) {
        const unsigned int shipId = static_cast<unsigned int>(payload.ship->id);
        const hlt::Position& shipyardPos = payload.game.me->shipyard->position;

        if (payload.ship->halite >= hlt::constants::MAX_HALITE) {
            returningShips.insert(shipId);
            return BehaviorTree::NodeState::Success;
        }

        if (returningShips.contains(shipId)) {
            if (payload.ship->position == shipyardPos) {
                returningShips.erase(shipId);
                return BehaviorTree::NodeState::Failure;
            }

            return BehaviorTree::NodeState::Success;
        }

        return BehaviorTree::NodeState::Failure;
    };
}

// --- ShipAIV2::ReturnToBase ---
ShipAIV2::ReturnToBase::ReturnToBase(BehaviorTree::Node<Payload>* parent) : Leaf(parent) {
    this->evaluation = [&](const Payload& payload) {
        // Navigate towards shipyard until arrival; IsFull handles clearing the returning flag.
        hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, payload.game.me->shipyard->position);
        
    	payload.commands.push_back(payload.ship->move(direction));
        return BehaviorTree::NodeState::Running;
    };
}

// --- ShipAIV2::IsOnRichCell ---
ShipAIV2::IsOnRichCell::IsOnRichCell(BehaviorTree::Node<Payload>* parent) : Leaf(parent) {
    this->evaluation = [&](const Payload& payload) {
        return payload.game.game_map->at(payload.ship->position)->halite >= 100 ? BehaviorTree::NodeState::Success : BehaviorTree::NodeState::Failure;
    };
}

// --- ShipAIV2::CollectHalite ---
ShipAIV2::CollectHalite::CollectHalite(BehaviorTree::Node<Payload>* parent) : Leaf(parent) {
    this->evaluation = [&](const Payload& payload) {
        // Do nothing, ship will collect halite by staying still
        payload.commands.push_back(payload.ship->stay_still());
        return BehaviorTree::NodeState::Running;
    };
}

// --- ShipAIV2::MoveToBestHalite ---
ShipAIV2::MoveToBestHalite::MoveToBestHalite(BehaviorTree::Node<Payload>* parent) : Leaf(parent) {
    this->evaluation = [&](const Payload& payload) {
        int currentTurn = payload.game.turn_number;

        if (currentTurn != lastProcessedTurn) {
            targetedCells.clear();
            lastProcessedTurn = currentTurn;
        }

        const unsigned int width = payload.game.game_map->width;
        const unsigned int height = payload.game.game_map->height;

        hlt::Position bestPosition = payload.ship->position;
        double bestScore = -std::numeric_limits<double>::infinity();
        unsigned int bestDistance = std::numeric_limits<unsigned int>::max();

        for (unsigned int y = 0; y < height; ++y) {
            for (unsigned int x = 0; x < width; ++x) {
	            constexpr double targetedPenalty = 900.0;
	            constexpr double distancePenalty = 8.0;

	            hlt::MapCell* cell = payload.game.game_map->at({ (int) x, (int) y });

                if (cell->has_structure()) {
                    continue;
                }

                unsigned int distance = (unsigned int) payload.game.game_map->calculate_distance(payload.ship->position, { (int) x, (int) y});
                unsigned int index = y * width + x;

                double halite = cell->halite;
                bool alreadyTargeted = targetedCells.contains(index);

                double score = halite - (double) distance * distancePenalty - (alreadyTargeted ? targetedPenalty : 0.0);

                if (score > bestScore || score == bestScore && distance < bestDistance) {
                    bestScore = score;
                    bestPosition = cell->position;
                    bestDistance = distance;
                }
            }
        }

        unsigned int chosenIndex = (unsigned int) bestPosition.y * width + (unsigned int) bestPosition.x;
        targetedCells.insert(chosenIndex);

        hlt::Direction direction = payload.game.game_map->naive_navigate(payload.ship, bestPosition);


    	payload.commands.push_back(payload.ship->move(direction));
        return BehaviorTree::NodeState::Running;
    };
}

// --- Behavior Tree Construction ---
BehaviorTree::Selector<Payload> ShipAIV2::GetBehaviorTree() {
    static BehaviorTree::Selector<Payload> root(nullptr);

    static BehaviorTree::Sequencer<Payload> returnIfFull(&root);
    static IsFull isFull(&returnIfFull);
    static ReturnToBase returnToBase(&returnIfFull);

    static BehaviorTree::Sequencer<Payload> collectIfRich(&root);
    static IsOnRichCell isOnRichCell(&collectIfRich);
    static CollectHalite collectHalite(&collectIfRich);

    static MoveToBestHalite moveToBestHalite(&root);
    return root;
}