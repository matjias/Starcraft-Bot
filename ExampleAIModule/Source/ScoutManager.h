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
#include <BWAPI.h>
#include <vector>
#include <unordered_map>

class DLL_SPECIFIER ScoutManager {
public:
	ScoutManager();
	ScoutManager(BWAPI::Game *gameWrapper);
	~ScoutManager();

	// Init function
	bool _init(BWAPI::TilePosition::list allSpawns, BWAPI::TilePosition ownSpawn);
	bool _initT();

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
	};

	// Debug functions
	BWAPI::TilePosition::list getSpawns();
	std::vector<bool> getSpawnBools();

private:
	BWAPI::Game* broodwar;

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
