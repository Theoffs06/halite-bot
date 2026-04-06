#include "include/unit_manager.hpp"
#include "include/weighted_picker.hpp"
#include "include/util.hpp"

UnitManager::UnitManager(unsigned int rngSeed) {
	this->rng = std::mt19937(rngSeed);
}

void UnitManager::TryAddUnit(hlt::EntityId id) {
	if (!unitTypes.contains(id)) {
		WeightedPicker<UnitType> picker;
		picker.Add(UnitType::Miner, MINER_PERCENTAGE);
		picker.Add(UnitType::Explorer, EXPLORER_PERCENTAGE);

		unitTypes[id] = picker.Pick(rng);
	}
}

UnitType UnitManager::GetUnitType(hlt::EntityId id) {
	if (!unitTypes.contains(id)) return UnitType::Miner;
	return unitTypes[id];
}