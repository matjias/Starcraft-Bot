#pragma once

#include <BWAPI.h>
#include <map>

class BuildingUnits
{
public:
	BuildingUnits();
	~BuildingUnits();

	void addBuilding(BWAPI::Unit unit);
	bool deleteUnit(BWAPI::Unit unit);
	int getBuildingCount(BWAPI::UnitType unitType);
	BWAPI::Unit getIdleBuilding(BWAPI::UnitType unitType);

private:
	std::multimap<BWAPI::UnitType, BWAPI::Unit> buildings;
};
