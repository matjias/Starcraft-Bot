#include "BuildingUnits.h"

BuildingUnits::BuildingUnits() {
}

BuildingUnits::~BuildingUnits() {
}

void BuildingUnits::addBuilding(BWAPI::Unit unit) {
	buildings.insert(std::make_pair(unit->getType(), unit));
}

bool BuildingUnits::deleteUnit(BWAPI::Unit unit) {
	int curAmount = buildings.count(unit->getType());
	for (std::map<BWAPI::UnitType, BWAPI::Unit>::iterator it = buildings.lower_bound(unit->getType()); it != buildings.upper_bound(unit->getType()); it++){
		if (it->second == unit){
			buildings.erase(it);
			return curAmount > buildings.count(unit->getType());
		}
	}
	return false;
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
