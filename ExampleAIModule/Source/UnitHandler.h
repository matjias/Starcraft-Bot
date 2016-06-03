#pragma once
#include "BWAPI.h"
#include "CombatUnits.h"
#include "ProbeUnits.h"
#include "BuildingUnits.h"

class UnitHandler {
public:
	UnitHandler();
	~UnitHandler();
	void _init();
	void addUnit(BWAPI::Unit u);
	void addWarpingUnit(BWAPI::Unit u);
	void removeWarpingUnit(BWAPI::Unit u);
	int getWarpingUnitCount(BWAPI::UnitType unitType);

	BuildingUnits* getBuildingUnits();

private:
	CombatUnits combatUnits;
	ProbeUnits probeUnits;
	BuildingUnits buildingUnits;

	// Currently functioning as default unitset for probes.
	BWAPI::Unitset miningProbes;

	std::map<BWAPI::UnitType, BWAPI::Unit> warpingUnits;

	bool isCombatUnit(BWAPI::Unit u);
	bool isProbeUnit(BWAPI::Unit u);
};
