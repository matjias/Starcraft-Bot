#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include <vector>
#include <map>
#include <limits>
#include "ExampleAIModule.h"
#include "CustomVector.h"

class Scouting {
public:
	Scouting();
	~Scouting();
	void _init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc);
	bool isScouting();
	bool assignScout(BWAPI::Unit scout);
	bool hasAssignedScout();
	void updateScout();
	bool endScouting();
	int getDistance();
	void foundEnemyBase(BWAPI::TilePosition loc);
	void scoutHasDied();
	bool returnFoundEnemyBase();
	BWAPI::Position returnEnemyBaseLocs();

	// Enemy expansion functions
	void findEnemyExpansions();
	BWTA::BaseLocation* closestEnemyExpansion();
	std::vector<BWTA::BaseLocation*> closestEnemyExpansions();

	// BWTA finished analyzing fuction
	void set_BWTA_Analyzed();


	/*----- Start of Structs -----*/
	struct BuildingStruct {
		BWAPI::UnitType unit;
		int scoutedTime; // Represented in game ticks since start
	};

	struct UnitStruct {
		std::vector<BWAPI::Unit> unit;
		int scoutedTime;
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

	// Methods for maintaining and accessing enemy units and structures
	// that have been scouted thus far
	void recordUnitDiscover(BWAPI::Unit u, BWAPI::TilePosition loc, int timeTick);
	void recordUnitDestroy(BWAPI::Unit u, BWAPI::TilePosition loc);
	std::map<BWAPI::TilePosition, BuildingStruct*, CustomMapCompare> getEnemyStructures();
	std::map<BWAPI::UnitType, UnitStruct*> getEnemyUnits();
	int getEnemyStructureCount(BWAPI::UnitType u);
	void enemyBaseDestroyed();

	

	// DEBUG METHODS BELOW
	BWAPI::Position getPos();	// Returns the Position for the
								// current location to be scouted.

	bool isScout(BWAPI::Unit u);

	BWAPI::Position getScout();
	BWAPI::TilePosition::list getDynamicSpawns();
	std::vector<bool> getDynamicSpawnBools();

private:
	// Private functions that the class uses
	void updateToScoutList();
	void distractEnemyBase();
	void validMove(BWAPI::Unit unitToMove, BWAPI::Position targetLoc);
	void validAttack(BWAPI::Unit unitToAttack, BWAPI::Unit targetUnit);


	// Global variables needed for Scouting
	BWAPI::Unit currentScout;
	bool hasScout, foundEnemy, BWTA_isAnalyzed;
	std::vector<LocationStruct*> dynamicLocations;
	BWAPI::TilePosition enemyBaseLoc;

	BWTA::Region *enemyRegion;
	BWTA::BaseLocation *enemyBaseLocation;
	BWTA::BaseLocation *enemyExpansion;
	std::vector<BWTA::BaseLocation*> enemyExpansions;

	std::map<BWAPI::TilePosition, BuildingStruct*, CustomMapCompare> enemyStructures;
	std::map<BWAPI::UnitType, UnitStruct*> enemyUnits;
};