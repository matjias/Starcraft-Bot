#pragma once
#include "BWAPI.h"
#include "Squad.h"
#include <math.h>


class CombatUnits {
public:
	CombatUnits();
	~CombatUnits();

	void _init();
	void update();
	void runAttack(BWAPI::Position attackPos);
	void addUnit(BWAPI::Unit u);
	bool deleteUnit(BWAPI::Unit unit);
	int getUnitCount(BWAPI::UnitType unitType);
	void setAttacking();
	void setAnalyzed(BWAPI::Position pos);

private:
	const int SQUAD_SIZE = 4;
	const int POS_RADIUS = 8;
	const int ZEALOT_RUSH_COUNT = 12;

	int zealotCount;
	bool mapAnalyzed;
	bool attacking = false;

	BWAPI::Position rendezvousPos;

	// Probably should be united to one multimap based on unit type since we are focusing on expandability.
	// too static and not enough flexibility for the tactician.
	std::multimap<BWAPI::UnitType, Squad> unitMap;

	void idleUpdate();
	void dragoonMicro(Squad *squad);
	void corsairMicro(Squad *squad);
	void idleMovement(Squad *squad, BWAPI::Position idleLoc);
	void idleMovement(BWAPI::Unit u, BWAPI::Position idleLoc);
	void attackMovement(Squad *squad, BWAPI::Position pos);
	void saveUnitToSquad(BWAPI::Unit u);
	bool unitAtPos(BWAPI::Unit u, BWAPI::TilePosition pos);
	bool squadAtPos(Squad uSet, BWAPI::TilePosition pos);
	bool enemyTooClose(BWAPI::Unit unit);

	BWAPI::Unit getOptimalTarget(BWAPI::Unit unit);

	void debugDraw(std::vector<Squad> squads);

	BWAPI::Position escapePos(BWAPI::Unit unit);
};
