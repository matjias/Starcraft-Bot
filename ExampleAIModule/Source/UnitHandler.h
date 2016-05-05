#pragma once
#include "BWAPI.h"
#include "CombatUnits.h"

class UnitHandler {
public:
	UnitHandler();
	~UnitHandler();
	void _init();
	void addUnit(BWAPI::Unit u);

private:
	CombatUnits combatUnits;

	bool isCombatUnit(BWAPI::Unit u);

};
