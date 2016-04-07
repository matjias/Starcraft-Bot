#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include <vector>
#include <map>
#include "ExampleAIModule.h"

class Scouting {
public:
	Scouting();
	~Scouting();
	void _init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc, ExampleAIModule* mainProg);
	bool isScouting();
	bool assignScout(BWAPI::Unit scout);
	bool hasAssignedScout();
	bool assignScoutandLoc(BWAPI::Unit scout, BWAPI::Position loc);
	void updateScout();
	bool endScouting();
	int getDistance();
	void foundEnemyBase(BWAPI::TilePosition loc);
	void scoutHasDied();
	bool returnFoundEnemyBase();
	BWAPI::Position returnEnemyBaseLocs();
	void requestScout();

	// BWTA finished analyzing fuction
	void set_BWTA_Analyzed();


	/*----- Start of Structs -----*/
	struct BuildingStruct {
		BWAPI::UnitType unit;
		BWAPI::TilePosition location;
		int scoutedTime; // Represented in game ticks since start
	};

	struct CustomMapCompare {
		bool operator()(const BWAPI::TilePosition& pos1, const BWAPI::TilePosition& pos2) const {
			// Compares the 2 positions and returns the true if the pos1
			// is the shortest distance to (0,0)
			// The position can be changed at a later time for efficiency?
			return sqrt(pow(pos1.x, 2) + pow(pos1.y, 2)) < sqrt(pow(pos2.x, 2) + pow(pos2.y, 2));
		}
	};

	struct LocationStruct {
		BWAPI::Position location;
		bool scouted;
	};
	/*----- End of Structs -----*/

	void recordUnitDiscover(BWAPI::UnitType u, BWAPI::TilePosition loc, int timeTick);
	void recordUnitDestroy(BWAPI::UnitType u, BWAPI::TilePosition loc);
	std::map<BWAPI::TilePosition, Scouting::BuildingStruct*, CustomMapCompare> getEnemyStructures();
	void enemyBaseDestroyed();

	

	// DEBUG METHODS BELOW
	BWAPI::Position getPos();	// Returns the Position for the
								// current location to be scouted.

	bool isScout(BWAPI::Unit u);

	BWAPI::Position getScout();
	BWAPI::TilePosition::list getDynamicSpawns();
	std::vector<bool> getDynamicSpawnBools();

private:
	// Private functions that Scouting uses
	void oneScoutAll(BWAPI::Unit u);
	void updateToScoutList();
	void distractEnemyBase();


	// Global variables needed for Scouting
	BWAPI::Unit currentScout;
	bool hasScout, foundEnemy, BWTA_isAnalyzed;
	std::vector<Scouting::LocationStruct*> dynamicLocations;
	std::map<BWAPI::TilePosition, Scouting::BuildingStruct*, Scouting::CustomMapCompare> enemyStructures;
	BWAPI::TilePosition enemyBaseLoc;

	// The main program class
	ExampleAIModule* mainProgram;
};