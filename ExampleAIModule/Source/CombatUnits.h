#pragma once
#include "BWAPI.h"
#include "Squad.h"


class CombatUnits {
public:
	CombatUnits();
	~CombatUnits();

	void _init();
	void idleUpdate();
	void addUnit(BWAPI::Unit u);

private:
	const int SQUAD_SIZE = 4;
	const int POS_RADIUS = 8;
	const int ZEALOT_RUSH_COUNT = 12;

	int zealotCount;
	bool mapAnalyzed;

	BWAPI::Position ownChoke;
	BWAPI::Position enemyChoke;
	BWAPI::Position attackLoc;

	std::vector<Squad> zealotSquads;
	std::vector<Squad> dragoonSquads;

	void idleMovement(Squad *squad, BWAPI::Position idleLoc);
	void attackMovement(Squad *squad);
	void saveUnitToSquad(BWAPI::Unit u, std::vector<Squad> *squads);
	bool unitAtPos(BWAPI::Unit u, BWAPI::TilePosition pos);
	bool squadAtPos(Squad uSet, BWAPI::TilePosition pos);

	void debugDraw(std::vector<Squad> squads);

	BWAPI::Position escapePos(Unit unit);
};
