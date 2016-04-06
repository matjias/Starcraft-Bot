#include "Army.h"
#include "Constants.h"
#include "Scouting.h"

using namespace BWAPI;

std::vector<Unit> zealots; // Dead Zealots should be removed from the vector!
std::vector<Unit> dragoons;

BWAPI::Position enemyBaseLocs;

Position idleLoc;
bool mapAnalyzed;

Army::Army(){}

Army::~Army(){}

void Army::_init(){
	mapAnalyzed = false;
}

void Army::update(Scouting scoutClass){
	if (mapAnalyzed){
		idleLoc = BWTA::getNearestChokepoint(Position(Broodwar->self()->getStartLocation()))->getCenter();
	}
	enemyBaseLocs = scoutClass.returnEnemyBaseLocs();
	
	if (zealots.size() > ZEALOT_RUSH_SIZE) {
		attack();
	}
	else {
		for (int i = 0; i < zealots.size(); i++){
			//zealots.at(i)->move(idleLoc);
			if (zealots.at(i)->isIdle()){
				zealots.at(i)->attack(idleLoc);
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
		if (u->isIdle()){
			u->attack(enemyBaseLocs);
		}
	}
}

bool Army::buildZealot(BWAPI::Unit u){
	return u->train(UnitTypes::Protoss_Zealot);
}

bool Army::buildDragoon(BWAPI::Unit u){
	return u->train(UnitTypes::Protoss_Dragoon);
}

void Army::addZealot(BWAPI::Unit u){
	zealots.push_back(u);
}

void Army::addDragoon(BWAPI::Unit u){
	dragoons.push_back(u);
}

void Army::setAnalyzed(bool analyzed){
	mapAnalyzed = analyzed;
}
