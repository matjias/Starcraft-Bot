#pragma once

#include <BWAPI.h>
#include <map>

class BuildingUnits
{
public:
	BuildingUnits();
	~BuildingUnits();

	void addBuilding(BWAPI::Unit unit);
	void removeBuilding(BWAPI::Unit unit);
	int getBuildingCount(BWAPI::UnitType unitType);
	BWAPI::Unit getIdleBuilding(BWAPI::UnitType unitType);

private:
	std::map<BWAPI::UnitType, BWAPI::Unit> buildings;
	std::map<BWAPI::UnitType, BWAPI::Unit>::iterator buildingsIterator = buildings.begin();
};
