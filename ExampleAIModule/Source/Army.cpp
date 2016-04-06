#include "Army.h"
#include "Constants.h"
#include "Scouting.h"

using namespace BWAPI;

std::vector<Unit> zealots; // Dead Zealots should be removed from the vector!

BWAPI::Position enemyBaseLocs;

Position idleLoc, enemyChoke;
bool mapAnalyzed;
int moved = 0;
int countAtEnemyChoke = 0;

Army::Army(){}

Army::~Army(){}

void Army::_init(){
	mapAnalyzed = false;
}

void Army::update(Scouting scoutClass){
	if (mapAnalyzed){
		idleLoc = BWTA::getNearestChokepoint(Position(Broodwar->self()->getStartLocation()))->getCenter();
		enemyChoke = BWTA::getNearestChokepoint(scoutClass.returnEnemyBaseLocs())->getCenter();
	}
	enemyBaseLocs = scoutClass.returnEnemyBaseLocs();
	
	if (zealots.size() > ZEALOT_RUSH_SIZE) {
		attack();
	}
	else {
		for (int i = 0; i < zealots.size(); i++){
			if (zealots.at(i)->canAttackMove() && zealots.at(i)->isIdle() && !zealotAtPos(zealots.at(i), TilePosition(idleLoc))){
				zealots.at(i)->move(idleLoc);
				moved++;
			}

		}
	}
}

void Army::attack(){
	zealotRush();
}

void Army::attack(Scouting scoutClass){
	enemyBaseLocs = scoutClass.returnEnemyBaseLocs();
	zealotRush();
}

void Army::zealotRush(){
	for (Unit u : zealots){
		if (zealotAtPos(u, TilePosition(enemyChoke))){
			countAtEnemyChoke++;
		}
		if (zealotAtPos(u, TilePosition(enemyChoke)) && countAtEnemyChoke > ZEALOT_RUSH_SIZE){
			u->attack(enemyBaseLocs);
		}
		else if (u->canAttackMove() && u->isIdle() && !zealotAtPos(u, TilePosition(enemyChoke))){
			u->move(enemyChoke);
			moved++;
		}
	}
}

bool Army::buildZealot(BWAPI::Unit u){
	return u->train(UnitTypes::Protoss_Zealot);
}

void Army::addZealot(BWAPI::Unit u){
	zealots.push_back(u);
}

void Army::setAnalyzed(bool analyzed){
	mapAnalyzed = analyzed;
}

std::vector<BWAPI::Unit> Army::getZealots(){
	return zealots;
}

bool Army::zealotAtPos(Unit zealot, TilePosition pos){
	return zealot->getTilePosition().x > pos.x - 6
		&& zealot->getTilePosition().x < pos.x + 6
		&& zealot->getTilePosition().y > pos.y - 6
		&& zealot->getTilePosition().y < pos.y + 6;
}