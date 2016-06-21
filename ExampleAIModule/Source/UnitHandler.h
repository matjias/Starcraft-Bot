#ifdef UNITHANDLER_EXPORTS
#	define DLL_SPECIFIER _declspec(dllexport)
#else
#	define DLL_SPECIFIER _declspec(dllimport)
#endif

#pragma once
#include "BWAPI.h"
#include "BWTA.h"
#include "CombatUnits.h"
#include "ProbeUnits.h"
#include "BuildingUnits.h"
#include "ScoutUnits.h"

//class ScoutUnits;

class DLL_SPECIFIER UnitHandler {
public:
	UnitHandler();
	~UnitHandler();
	void _init();
	void setBroodwarMock(BWAPI::Game* mockBroodwarPtr);

	void addUnit(BWAPI::Unit u);
	void addWarpingUnit(BWAPI::Unit u);
	void removeWarpingUnit(BWAPI::Unit u);
	int getWarpingUnitCount(BWAPI::UnitType unitType);
	bool purchaseUnit(BWAPI::UnitType unitType);
	bool purchaseBuilding(BWAPI::UnitType building);
	bool purchaseUpgrade(BWAPI::UpgradeType upgradeType);
	bool deleteUnit(BWAPI::Unit);
	bool addScout(BWAPI::UnitType unitType);
	float getArmyValue();
	void setArmyBalance(float value);
	void update();

	BuildingUnits* getBuildingUnits();
	CombatUnits* getCombatUnits();
	ProbeUnits* getProbeUnits();
	ScoutUnits* getScoutUnits();

	void setAnalyzed();

private:
	
	enum UnitPlacement{
		combat,
		probe,
		scout,
		building,
	};

	int armyValue = 0;
	
	std::map<int, UnitPlacement> units;
	CombatUnits combatUnits;
	ProbeUnits probeUnits;
	BuildingUnits buildingUnits;
	ScoutUnits scoutUnits;

	std::multimap<BWAPI::UnitType, BWAPI::Unit> warpingUnits;

	bool isCombatUnit(BWAPI::Unit u);
	bool isProbeUnit(BWAPI::Unit u);
	bool isBuilding(BWAPI::Unit u);

	float armyBalance = DEFAULT_ARMY_BALANCE;
};