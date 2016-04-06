#pragma once
#include <BWAPI.h>
#include <vector>
#include <map>

class Scouting {
public:
	Scouting();
	~Scouting();
	void _init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc);
	bool isScouting();
	bool assignScout(BWAPI::Unit scout);
	bool hasAssignedScout();
	bool assignScoutandLoc(BWAPI::Unit scout, BWAPI::Position loc);
	void updateScout();
	bool endScouting();
	int getDistance();
	void foundEnemyBase(BWAPI::TilePosition loc);
	void scoutHasDied();


	// Structs
	struct BuildingStruct {
		BWAPI::UnitType unit;
		BWAPI::TilePosition location;
		int scoutedTime; // Represented in game ticks since start
	};

	struct LocationStruct {
		BWAPI::Position location;
		bool scouted;
	};

	void recordUnitDiscover(BWAPI::UnitType u, BWAPI::TilePosition loc, int timeTick);
	void recordUnitDestroy(BWAPI::UnitType u, BWAPI::TilePosition loc);
	std::map<BWAPI::TilePosition, Scouting::BuildingStruct*> getEnemyStructures();
	void enemyBaseDestroyed();

	


	// DEBUG METHODS BELOW

	int getX();
	int getY();

	bool isScout(BWAPI::Unit u);

	BWAPI::Position getScout();


	BWAPI::TilePosition::list getSpawns();
	BWAPI::TilePosition::list getDynamicSpawns();
	std::vector<bool> getDynamicSpawnBools();

	bool returnFoundEnemyBase();
	BWAPI::Position returnEnemyBaseLocs();

private:
	void oneScoutAll(BWAPI::Unit u);
	void updateToScoutList();
	void distractEnemyBase();
};