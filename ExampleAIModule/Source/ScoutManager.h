// Dll exports to allow for unit testing
#ifdef SCOUTMANAGER_EXPORTS
#	define DLL_SPECIFIER _declspec(dllexport)
#else
#	define DLL_SPECIFIER _declspec(dllimport)
#endif

// Disable dll-interface warnings to private members
#pragma warning(disable : 4251)

// Now actual program begins
#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_map>

#include "BWAPI.h"
#include "Constants.h"
#include "ScoutUnits.h"


class DLL_SPECIFIER ScoutManager {
public:
	ScoutManager();
	~ScoutManager();

	// Init function
	bool _init();

	// Unit Testing functions, used for unit testing
	void setBroodwarMock(BWAPI::Game* broodwarPtr);
	BWAPI::Player testBroodwar();

	// Information tracking
	void recordUnitDiscover(BWAPI::Unit u);
	int recordUnitDestroy(BWAPI::Unit u);
	void recordUnitEvade(BWAPI::Unit u);

	// ScoutManager gets information from somewhere
	void updateScoutManager();

	

	// Unit structure, used for caching enemy units in a map
	struct UnitStruct {
		BWAPI::Unit unit;
		BWAPI::Position lastKnownPosition;
		int lastScouted;
	};

	// Location structure, used for saving which locations
	// have been scouted
	struct LocationStruct {
		BWAPI::TilePosition location;
		bool scouted = false;
		bool hasScout = false;
	};

	// Regular scouting functions
	bool beginScouting(ScoutUnits* scoutUnitsPtr);
	bool foundEnemyBase(BWAPI::TilePosition position);

	// Tell the tactician that it can add another scout
	// if tactician deems it a good idea
	bool canAddAnotherScout();

	// Debug functions
	BWAPI::TilePosition::list getSpawns();
	std::vector<bool> getSpawnBools();
	std::vector<bool> getSpawnHasScouts();
	std::vector<ScoutAndGoalStruct*> getScouts();
	BWAPI::TilePosition getEnemySpawn();
	bool hasScouts();
	bool isScouting = false;

private:
	ScoutUnits* scoutUnits = nullptr;

	// Private functions
	bool foundEnemySpawn = false;
	bool peekEnemySpawn = false;
	bool peekEnemyExpansions = false;
	BWAPI::TilePosition enemySpawn = BWAPI::TilePosition(0,0);

	void findEnemySpawn();
	void updateSpawnList();

	bool clearUnusedScouts();

	void scoutPeekEnemySpawn();

	bool foundEnemyBaseInit(BWAPI::TilePosition position);

	/*bool scoutHasGoal(ScoutAndGoalStruct* scoutAndGoal);
	LocationStruct* getScoutGoalInSpawns(BWAPI::TilePosition goal);
	bool haveAnyScoutsReachedGoal(std::vector<ScoutAndGoalStruct*> scoutsAndGoals);
	bool updateGoals(std::vector<ScoutAndGoalStruct*> scoutsAndGoals);
	void assignAGoal(ScoutAndGoalStruct* scoutAndGoal);
	bool hasACloserGoal(ScoutAndGoalStruct* scoutAndGoal);*/

	// Using an unordered map to keep track of enemy units
	// in average constant time, using the unit ID as the
	// key and the element containing various other information
	std::unordered_map<int, UnitStruct*> enemyUnits;

	// A vector of locations, used for caching information about
	// spawn locations and which ones have been scouted thus far.
	// This list is at runtime, and when the scout moves to optimize
	// the process of finding the enemy.
	std::vector<LocationStruct*> spawns;


};
