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
	void addUnit(BWAPI::Unit u);
	int getUnitCount(BWAPI::UnitType unitType);
	void setAttacking(BWAPI::Position pos);
	void setAnalyzed();

private:
	const int SQUAD_SIZE = 4;
	const int POS_RADIUS = 8;
	const int ZEALOT_RUSH_COUNT = 12;

	int zealotCount;
	bool mapAnalyzed;
	bool attacking = false;

	BWAPI::Position ownChoke;
	BWAPI::Position enemyChoke;
	BWAPI::Position attackLoc;

	// Probably should be united to one multimap based on unit type since we are focusing on expandability.
	// too static and not enough flexibility for the tactician.
	std::vector<BWAPI::Unit> units;
	std::vector<Squad> zealotSquads;
	std::vector<Squad> dragoonSquads;

	void idleUpdate();
	void runAttack();
	void dragoonMicro(Squad squad);
	void idleMovement(Squad *squad, BWAPI::Position idleLoc);
	void idleMovement(BWAPI::Unit u, BWAPI::Position idleLoc);
	void attackMovement(Squad *squad);
	void saveUnitToSquad(BWAPI::Unit u, std::vector<Squad> *squads);
	bool unitAtPos(BWAPI::Unit u, BWAPI::TilePosition pos);
	bool squadAtPos(Squad uSet, BWAPI::TilePosition pos);
	bool enemyTooClose(BWAPI::Unit unit);

	BWAPI::Unit getOptimalTarget(BWAPI::Unit unit);

	void debugDraw(std::vector<Squad> squads);

	BWAPI::Position escapePos(BWAPI::Unit unit);
};
