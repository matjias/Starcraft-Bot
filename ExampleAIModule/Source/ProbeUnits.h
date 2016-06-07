#ifdef UNITHANDLER_EXPORTS
#	define DLL_SPECIFIER _declspec(dllexport)
#else
#	define DLL_SPECIFIER _declspec(dllimport)
#endif

#pragma once
#include "BWAPI.h"
#include "BWTA.h"



class DLL_SPECIFIER ProbeUnits {
public:
	ProbeUnits();
	~ProbeUnits();

	void addUnit(BWAPI::Unit u);
	int getWorkerCount();
	void increaseGasMiners(int amount);
	void decreaseGasMiners(int amount);
	void update();

	bool newBuilding(BWAPI::UnitType type);
	bool newBuilding(BWAPI::UnitType type, BWAPI::TilePosition pos);

	BWAPI::Unitset* getMiningUnits();
	BWAPI::Unit extractUnit();

private:
	int miningCount;

	// Currently functioning as default unitset for probes.
	BWAPI::Unitset miningProbes;
	BWAPI::Unitset gasProbes;

	void mineMinerals(BWAPI::Unitset uSet);
	void mineMinerals(BWAPI::Unit u);
	void mineGas(BWAPI::Unitset uSet);

	void moveUnits(BWAPI::Unitset *setFrom, BWAPI::Unitset *setTo, int amount);
	BWAPI::TilePosition getOptimalBuildPlacement(BWAPI::UnitType type, BWAPI::TilePosition basePos);
	bool checkMargin(BWAPI::UnitType type, BWAPI::TilePosition basePos);
};