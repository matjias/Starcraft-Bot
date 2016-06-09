#pragma once
#include "BWAPI.h"

enum StrategyName {
	// Default has no direct goal in mind, but just works towards
	// getting economy, teching up, building an army and harassing
	// where possible
	Default, Skirmish,
	AllIn, Defend,
	Expand, Turtle
};

enum GameStage {
	Start, Early, EarlyMid,
	Mid, MidLate, Late
};

struct ScoutAndGoalStruct {
	BWAPI::Unit scout;
	BWAPI::Position goal = BWAPI::Position(0, 0);
	bool reachedGoal = false;
};

// Economy
const int MAX_SUPPLY = 200;
const int WORKERS_PER_MINERAL_LINE = 9 * 2; // 9 Mineral Fields, 2 workers per Mineral Field
const int WORKERS_PER_GEYSER = 3;
const int MAX_WORKERS = (WORKERS_PER_MINERAL_LINE + WORKERS_PER_GEYSER) * 4; // Max 4 fully mining bases
const int MAX_WORKER_SURPLUS = WORKERS_PER_MINERAL_LINE + WORKERS_PER_GEYSER;
const int MINERAL_SURPLUS_LIMIT = 600;
const int GAS_SURPLUS_LIMIT = 400;

// Units and buildings
const int MIN_OBSERVERS = 1; // Observers needed to scout
const int OBSERVERS_TO_DETECT = 3; // Observers needed when cloak/burrow is spotted
const int DEFENSE_STRUCTURES_PER_BASE = 3;
const int CORSAIRS_NEEDED = 3;
const int ARMY_SUPPLY_BEFORE_OBSERVERS = 8;
const int ARMY_SUPPLY_BEFORE_CORSAIRS = 2;
const int ARMY_SUPPLY_BEFORE_UPGRADES = 10;
const int DRAGOONS_BEFORE_RANGE = 4;
const int ZEALOT_RUSH_SIZE = 12;
const int DRAGOON_RUSH_SIZE = 16;

// Scouting and map size
const int MEDIUM_MAP_SIZE = 128 * 128; // 128 * 128
const int RUSH_DISTANCE_CEIL = 128 * 32; // 128 * 32