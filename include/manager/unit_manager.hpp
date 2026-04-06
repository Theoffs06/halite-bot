#pragma once

#include <random>
#include <map>

#include "hlt/types.hpp"

// Define unit types for bot. Each unit can be a Miner or an Explorer.
enum class UnitType {
	Miner,
	Explorer,
};

// Manages unit types (Miner, Explorer) and assigns them randomly when new units are added.
class UnitManager {
public:
	// Initialize the UnitManager with a random seed for consistent unit type assignment.
	UnitManager(unsigned int rngSeed);

	// Try to add a unit with the given ID. If the unit is new, randomly assign it a type.
	void TryAddUnit(hlt::EntityId id);

	// Get the type of a unit by its ID. If the unit is not known, default to Miner.
	UnitType GetUnitType(hlt::EntityId id);
private:
	std::mt19937 m_rng;
	std::map<hlt::EntityId, UnitType> m_unitTypes;
};
