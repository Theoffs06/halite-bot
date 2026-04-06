#include "include/manager/unit_manager.hpp"
#include "include/weighted_picker.hpp"
#include "include/util.hpp"

// Initialize the UnitManager with a random seed for consistent unit type assignment.
UnitManager::UnitManager(unsigned int rngSeed) {
	this->m_rng = std::mt19937(rngSeed);
}

// Try to add a unit with the given ID. If the unit is new, randomly assign it a type.
void UnitManager::TryAddUnit(hlt::EntityId id) {
	if (!m_unitTypes.contains(id)) {
		WeightedPicker<UnitType> picker;
		picker.Add(UnitType::Miner, MINER_PERCENTAGE);
		picker.Add(UnitType::Explorer, EXPLORER_PERCENTAGE);

		m_unitTypes[id] = picker.Pick(m_rng);
	}
}

// Get the type of a unit by its ID. If the unit is not known, default to Miner.
UnitType UnitManager::GetUnitType(hlt::EntityId id) {
	if (!m_unitTypes.contains(id)) return UnitType::Miner;
	return m_unitTypes[id];
}