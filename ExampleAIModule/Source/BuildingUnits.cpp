#include "BuildingUnits.h"

BuildingUnits::BuildingUnits() {
}

BuildingUnits::~BuildingUnits() {
}

void BuildingUnits::addBuilding(BWAPI::Unit unit) {
	buildings.insert(std::make_pair(unit->getType(), unit));
}

void BuildingUnits::removeBuilding(BWAPI::Unit unit) {
	while (buildingsIterator != buildings.end()) {
		if (buildingsIterator->second == unit) {
			buildingsIterator = buildings.erase(buildingsIterator);
		}
		else {
			++buildingsIterator;
		}
	}
}

int BuildingUnits::getBuildingCount(BWAPI::UnitType unitType) {
	return buildings.count(unitType);
}

BWAPI::Unit BuildingUnits::getIdleBuilding(BWAPI::UnitType unitType) {
	int idleBuildings = 0;

	while (buildingsIterator != buildings.end()) {
		if (buildingsIterator->first == unitType &&
			buildingsIterator->second->isCompleted() &&
			!buildingsIterator->second->isTraining() &&
			!buildingsIterator->second->isResearching()) {

			return buildingsIterator->second;
		}
	}

	return NULL;
}
