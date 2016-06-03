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
	void queueUnit();

private:
	CombatUnits combatUnits;
	ProbeUnits probeUnits;

	// Currently functioning as default unitset for probes.
	BWAPI::Unitset miningProbes;

	bool isCombatUnit(BWAPI::Unit u);
	bool isProbeUnit(BWAPI::Unit u);
};
