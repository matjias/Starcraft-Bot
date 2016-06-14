#pragma once
#include "BWAPI.h"

enum StrategyName {
	// Default has no direct goal in mind, but just works towards
	// getting economy, teching up, building an army and harassing
	// where possible
	Default, // Build anything except for expansions, attack if there are any openings
	AllIn, // Only spend resources on army and army structures, attack with army when there are openings or the army is "big"
	Defend, // Send main army back, spend resources on combat units and defense structures
	Expand // Send main army to expansion, spend resources on a new base first and then on anything else
};

enum GameStage {
	Start,
	Early,
	Mid, 
	Late
};

struct ScoutAndGoalStruct {
	BWAPI::Unit scout;
	BWAPI::Position goal = BWAPI::Position(0, 0);
	bool reachedGoal = false;
	bool peekEnemyBase = false;
	bool underAttack = false;
	bool kiting = false;
	int lastPolyPos = -1;
};

// Strategy
const int EARLY_STAGE_SUPPLY = 6;
const int MID_STAGE_SUPPLY = 50;
const int STRATEGY_UPDATE_TIME = 500;
const float ARMY_ATTACK_POWER_BALANCE = 0.7; // 0 = no army, 1 = enemy has no army
const float ARMY_RETREAT_POWER_BALANCE = 0.4;
const float ALLIN_WORKER_BALANCE = 0.2;

// Macro and economy
const int MAX_SUPPLY = 200;
const int WORKERS_PER_MINERAL_LINE = 9 * 2; // 9 Mineral Fields, 2 workers per Mineral Field
const int WORKERS_PER_GEYSER = 3;
const int MAX_WORKERS = 100;
const int MAX_WORKER_SURPLUS = WORKERS_PER_MINERAL_LINE + WORKERS_PER_GEYSER;
const int MINERAL_SURPLUS_LIMIT = 600;
const int GAS_SURPLUS_LIMIT = 400;
const float GAS_TO_MINERALS = 1.5;

// Units and buildings
const int OBSERVERS_TO_SCOUT = 1;
const int OBSERVERS_WITH_ARMY = 1;
const int DEFENSE_STRUCTURES_PER_BASE = 3;
const int DRAGOONS_BEFORE_RANGE = 4;
const int ZEALOT_RUSH_SIZE = 12;
const int DRAGOON_RUSH_SIZE = 16;

// Build location
const int BUILD_LOCATION_UPDATE_TIME = 50;

// Scouting, distances, and map size
const int MEDIUM_MAP_SIZE = 128 * 128;
const int RUSH_DISTANCE_CEIL = 128 * 32;
const int BUILD_LOCATION_FACTOR = 15;

// Old
const int MIN_OBSERVERS = 1;
const int OBSERVERS_TO_DETECT = 2;
const int CORSAIRS_NEEDED = 3;
const int ARMY_SUPPLY_BEFORE_OBSERVERS = 8;
const int ARMY_SUPPLY_BEFORE_CORSAIRS = 2;
const int ARMY_SUPPLY_BEFORE_UPGRADES = 10;
