#pragma once

#include <random>
#include <map>

#include "types.hpp"

enum class UnitType {
	Miner,
	Explorer,
};

class UnitManager {
public:
	UnitManager(unsigned int rngSeed);
	void TryAddUnit(hlt::EntityId id);
	UnitType GetUnitType(hlt::EntityId id);
private:
	std::mt19937 rng;
	std::map<hlt::EntityId, UnitType> unitTypes;
};
