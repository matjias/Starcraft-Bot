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

	void _init();
	void setBroodwarMock(BWAPI::Game* broodwarPtr);

	void addUnit(BWAPI::Unit u);
	int getWorkerCount();
	void increaseGasMiners(int amount);
	void decreaseGasMiners(int amount);
	void update();
	bool deleteUnit(BWAPI::Unit u);
	void mineNewBase(BWAPI::Unit mineralField);
	void mineGas(BWAPI::Unit base, BWAPI::Unit geyser);

	bool newBuilding(BWAPI::UnitType type);
	bool newBuilding(BWAPI::UnitType building, BWAPI::TilePosition pos);

	BWAPI::Unitset* getMiningUnits();
	BWAPI::Unit extractUnit();

	void setAnalyzed();

private:
	int workerCount = 0;
	bool mapAnalyzed;

	// Currently functioning as default unitset for probes.
	std::map<int, BWAPI::Unitset> miningProbes;
	std::map<int, BWAPI::Unitset> gasProbes;
	BWAPI::Unit builder;
	BWAPI::UnitType previousBuildingToBuild;
	int buildLocationCounter;

	void mineMinerals();
	void mineGas(BWAPI::Unit base);
	bool tileAvailableFromUnits(BWAPI::UnitType type, BWAPI::TilePosition tilePos);
	bool tileAvailableFromBuildings(BWAPI::TilePosition tilePos);

	void moveUnits(BWAPI::Unitset *setFrom, BWAPI::Unitset *setTo, int amount);
	BWAPI::TilePosition getOptimalBuildPlacement(BWAPI::UnitType type, BWAPI::TilePosition basePos);
	BWAPI::TilePosition recPlacement(BWAPI::UnitType type, BWAPI::TilePosition basePos, int depth);
	bool checkMargin(BWAPI::UnitType type, BWAPI::TilePosition basePos);
};