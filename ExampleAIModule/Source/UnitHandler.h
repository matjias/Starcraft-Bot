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
	void queueUnit(BWAPI::Unit u);
	void dequeueUnit(BWAPI::Unit u);
	std::unordered_map<int, BWAPI::Unit> getQueuedUnits();

private:
	CombatUnits combatUnits;
	ProbeUnits probeUnits;

	// Currently functioning as default unitset for probes.
	BWAPI::Unitset miningProbes;
	// Me like to play pool and billard wif dem Queues.
	std::unordered_map<int, BWAPI::Unit> cuedUnits;

	bool isCombatUnit(BWAPI::Unit u);
	bool isProbeUnit(BWAPI::Unit u);
};
