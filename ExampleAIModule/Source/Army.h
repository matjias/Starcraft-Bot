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
	void Army::attack(int atNum);
	void update(Scouting scoutClass);
	bool buildZealot(BWAPI::Unit u);
	bool buildDragoon(BWAPI::Unit u);
	bool buildObserver(BWAPI::Unit u);
	bool buildCorsair(BWAPI::Unit u);
	void addZealot(BWAPI::Unit u);
	void addDragoon(BWAPI::Unit u);
	void attack(Scouting scoutClass);
	void zealotRush();
	void setAnalyzed(bool analyzed);
	int moved, countAtEnemyChoke;
	bool unitAtPos(BWAPI::Unit zealot, BWAPI::TilePosition pos);
	bool squadAtPos(BWAPI::Unitset uSet, BWAPI::TilePosition pos);
	std::vector<BWAPI::Unit> getZealots();
	BWAPI::Position enemyChoke;
	void enemyBaseDestroyed();
	void addSquadMember(BWAPI::Unit u);
	void buildSquad(std::vector<std::pair<BWAPI::UnitType, int>> list);
	void combat();
	void Army::idleMovement(std::vector<BWAPI::Unitset> squads, std::vector<BWAPI::Unit> gens);
	void Army::attackMovement(std::vector<BWAPI::Unitset> squads, std::vector<BWAPI::Unit> gens);
	void Army::debugDraw(std::vector<BWAPI::Unitset> squads, std::vector<BWAPI::Unit> gens);
	BWAPI::Position Army::escapePos(BWAPI::Unit unit);
private:
	std::vector<BWAPI::Unit> zealotGens;
	std::vector<BWAPI::Unit> dragoonGens;

	std::vector<BWAPI::Unitset> zealotSquads;
	std::vector<BWAPI::Unitset> dragoonSquads;
	void attack();
};


