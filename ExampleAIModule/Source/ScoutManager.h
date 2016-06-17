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
#include "BWTA.h"
#include "Constants.h"
#include "ScoutUnits.h"

// Forward decleration
class StrategyDecider;

class DLL_SPECIFIER ScoutManager {
public:
	ScoutManager();
	~ScoutManager();

	// Init function
	bool _init();

	// Unit Testing functions, used for unit testing
	void setBroodwarMock(BWAPI::Game* broodwarPtr);
	BWAPI::Player testBroodwarReturnPlayer();

	// Information tracking
	void recordUnitDiscover(BWAPI::Unit u);
	int recordUnitDestroy(BWAPI::Unit u);
	void recordUnitEvade(BWAPI::Unit u);
	void recordUnitMorph(BWAPI::Unit u);

	// Getting information
	int getAmountOfEnemyUnit(BWAPI::UnitType u);
	std::map<BWAPI::UnitType, int> getEnemyUnitsAmount();
	int getEnemyBaseCount();
	int getEnemyWorkerCount();
	int getEnemyDefenseValue();
	float getEnemyArmyValue();
	BWAPI::Position getAnEnemyPosition();

	// ScoutManager gets information from somewhere
	void updateScoutManager();

	// Got signal to start searching expansions
	//bool initExpansions();
	

	// Unit structure, used for caching enemy units in a map
	struct UnitStruct {
		BWAPI::Unit unit;
		BWAPI::UnitType unitType;
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

	/*struct ExpansionStruct {
		BWTA::BaseLocation* base = NULL;
		bool scouted = false;
		bool hasScout = false;
		int lastScouted;
	};*/

	// Regular scouting functions
	bool setStrategyDecider(StrategyDecider* strategyPtr);
	bool beginScouting(ScoutUnits* scoutPtr);
	bool foundEnemyBase(BWAPI::TilePosition position);

	// Expansion functions
	/*bool hasScoutedAllExpansions();
	void resetAllExpansions();*/

	// Tell the tactician that it can add another scout
	// if tactician deems it a good idea
	bool canAddAnotherScout();

	// BWTA finished analyzing function
	void setAnalyzed();

	// Debug functions
	BWAPI::TilePosition::list getSpawns();
	std::vector<bool> getSpawnBools();
	std::vector<bool> getSpawnHasScouts();
	std::vector<ScoutAndGoalStruct*> getScouts();
	BWAPI::TilePosition getEnemySpawn();
	bool hasScouts();
	bool isScouting = false;

private:
	ScoutUnits* scoutUnitsPtr = nullptr;
	StrategyDecider* strategyDeciderPtr = nullptr;

	// Private functions
	int knownEnemyValue;
	bool foundEnemySpawn = false;
	bool peekEnemySpawn = false;
	bool peekEnemyExpansions = false;
	BWAPI::TilePosition enemySpawn = BWAPI::TilePosition(0,0);

	void findEnemySpawn();
	void updateSpawnList();

	bool clearUnusedScouts();

	void scoutPeekEnemySpawn();
	//void scoutPeekEnemyExpansions();

	bool foundEnemyBaseInit(BWAPI::TilePosition position);

	void incrementEnemyUnitsAmount(BWAPI::Unit u);
	void decrementEnemyUnitsAmount(BWAPI::Unit u);

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

	// To get information about the amount of certain unit
	// types the enemy have in average constant time,
	// takes the UnitType as the key and has the amount
	// discovered as the element
	std::map<BWAPI::UnitType, int> enemyUnitsAmount;

	// A vector of locations, used for caching information about
	// spawn locations and which ones have been scouted thus far.
	// This list is at runtime, and when the scout moves to optimize
	// the process of finding the enemy.
	std::vector<LocationStruct*> spawns;


	//std::vector<ExpansionStruct*> expansions;


	// BWTA finished analyzing variabla
	bool mapAnalyzed = false;
};
