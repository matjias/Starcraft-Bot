#pragma once
#include "BWAPI.h"
#include "CombatUnits.h"
#include "ProbeUnits.h"

class UnitHandler {
public:
	UnitHandler();
	~UnitHandler();
	void _init();
	void addUnit(BWAPI::Unit u);

private:
	CombatUnits combatUnits;
	ProbeUnits probeUnits;

	BWAPI::Unitset miningProbes;

	bool isCombatUnit(BWAPI::Unit u);
	bool isProbeUnit(BWAPI::Unit u);
};
