#ifdef UNITHANDLER_EXPORTS
#	define DLL_SPECIFIER _declspec(dllexport)
#else
#	define DLL_SPECIFIER _declspec(dllimport)
#endif

#pragma once
#include "BWAPI.h"
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
	void addUnit(BWAPI::Unit u);
	void addWarpingUnit(BWAPI::Unit u);
	void removeWarpingUnit(BWAPI::Unit u);
	int getWarpingUnitCount(BWAPI::UnitType unitType);
	void deleteUnit(BWAPI::Unit);

	void addScout(BWAPI::UnitType unitType);

	void update();

	BuildingUnits* getBuildingUnits();
	ProbeUnits* getProbeUnits();
	ScoutUnits* getScoutUnits();

	void setAnalyzed();

private:
	
	enum UnitPlacement{
		combat,
		probe,
		scout,
		building
	};
	
	std::map<int, UnitPlacement> units;
	CombatUnits combatUnits;
	ProbeUnits probeUnits;
	BuildingUnits buildingUnits;
	ScoutUnits scoutUnits;

	std::map<BWAPI::UnitType, BWAPI::Unit> warpingUnits;

	bool isCombatUnit(BWAPI::Unit u);
	bool isProbeUnit(BWAPI::Unit u);
	bool isBuilding(BWAPI::Unit u);
};