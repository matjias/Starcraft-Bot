#include "BuildingUnits.h"

BuildingUnits::BuildingUnits() {
}

BuildingUnits::~BuildingUnits() {
}

void BuildingUnits::addBuilding(BWAPI::Unit unit) {
	buildings.insert(std::make_pair(unit->getType(), unit));
}

void BuildingUnits::removeBuilding(BWAPI::Unit unit) {

	std::map<BWAPI::UnitType, BWAPI::Unit>::iterator buildingsIterator = buildings.begin();

	while (buildingsIterator != buildings.end()) {
		if (buildingsIterator->second == unit) {
			buildingsIterator = buildings.erase(buildingsIterator);
		}
		else {
			++buildingsIterator; // NOT buildingsIterator++;
		}
	}
}

int BuildingUnits::getBuildingCount(BWAPI::UnitType unitType) {
	return buildings.count(unitType);
}

BWAPI::Unit BuildingUnits::getIdleBuilding(BWAPI::UnitType unitType) {

	std::map<BWAPI::UnitType, BWAPI::Unit>::iterator buildingsIterator = buildings.begin();

	while (buildingsIterator != buildings.end()) {
		if (buildingsIterator->first == unitType &&
			!buildingsIterator->second->isTraining() &&
			!buildingsIterator->second->isResearching()) {
			return buildingsIterator->second;
		}
		buildingsIterator++;
	}

	return NULL;
}
