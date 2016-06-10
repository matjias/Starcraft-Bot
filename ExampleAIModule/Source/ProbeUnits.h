#ifdef UNITHANDLER_EXPORTS
#	define DLL_SPECIFIER _declspec(dllexport)
#else
#	define DLL_SPECIFIER _declspec(dllimport)
#endif

#pragma once
#include "BWAPI.h"
#include "BWTA.h"
#include "Constants.h"

class DLL_SPECIFIER ProbeUnits {
public:
	ProbeUnits();
	~ProbeUnits();

	void _init(BWAPI::Game* broodwarPtr);

	void addUnit(BWAPI::Unit u);
	int getWorkerCount();
	void increaseGasMiners(int amount);
	void decreaseGasMiners(int amount);
	void update();
	bool deleteUnit(BWAPI::Unit u);

	bool newBuilding(BWAPI::UnitType type);
	bool newBuilding(BWAPI::UnitType type, BWAPI::TilePosition pos);

	BWAPI::Unitset* getMiningUnits();
	BWAPI::Unit extractUnit();

	void setAnalyzed();

private:
	BWAPI::Game* broodwar;

	int workerCount = 0;
	bool mapAnalyzed;

	// Currently functioning as default unitset for probes.
	std::map<int, BWAPI::Unitset> miningProbes;
	std::map<int, BWAPI::Unitset> gasProbes;
	BWAPI::Unit builder;

	void mineMinerals();
	void mineNewBase(BWAPI::Unit mineralField);
	void mineGas(BWAPI::Unit base);
	bool unitBlocking(BWAPI::TilePosition basePos);

	void moveUnits(BWAPI::Unitset *setFrom, BWAPI::Unitset *setTo, int amount);
	BWAPI::TilePosition getOptimalBuildPlacement(BWAPI::UnitType type, BWAPI::TilePosition basePos);
	BWAPI::TilePosition recPlacement(BWAPI::UnitType type, BWAPI::TilePosition basePos, int depth);
	bool checkMargin(BWAPI::UnitType type, BWAPI::TilePosition basePos);
};