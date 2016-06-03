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

	BuildingUnits* getBuildingUnits();

private:
	CombatUnits combatUnits;
	ProbeUnits probeUnits;
	BuildingUnits buildingUnits;

	BWAPI::Unitset miningProbes;

	bool isCombatUnit(BWAPI::Unit u);
	bool isProbeUnit(BWAPI::Unit u);
};
