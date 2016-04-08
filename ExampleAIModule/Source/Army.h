#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include "Scouting.h"
#include "ExampleAIModule.h"


class Army
{
public:
	Army();
	~Army();

	void _init();

	void update(Scouting scoutClass);
	bool buildZealot(BWAPI::Unit u);
	bool buildDragoon(BWAPI::Unit u);
	void addZealot(BWAPI::Unit u);
	void addDragoon(BWAPI::Unit u);
	void attack(Scouting scoutClass);
	void zealotRush();
	void setAnalyzed(bool analyzed);
	int moved, countAtEnemyChoke;
	bool Army::zealotAtPos(BWAPI::Unit zealot, BWAPI::TilePosition pos);
	std::vector<BWAPI::Unit> getZealots();
	BWAPI::Position enemyChoke;
	void enemyBaseDestroyed();
	void Army::addSquadMember(int squadNumber, BWAPI::Unit u);
	void Army::buildSquad(std::vector<std::pair<BWAPI::UnitType, int>> list);
private:
	void attack();
};


