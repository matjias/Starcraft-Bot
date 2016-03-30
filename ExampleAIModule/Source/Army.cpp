#include "Army.h"
#include "Scouting.h"

using namespace BWAPI;

std::vector<Unit>zealots;

BWAPI::Position enemyBaseLocs;

const int ZEALOT_RUSH_SIZE = 8;
const int MAP_RADIUS = 1500;
Position startLoc;

Army::Army(){}

Army::~Army(){}


void Army::_init(){
	startLoc = Position(Broodwar->self()->getStartLocation());
	Broodwar->drawTextScreen(350, 100, "Zealot go to pos: %i, %i", startLoc.x, startLoc.y);
	if (startLoc.x > MAP_RADIUS && startLoc.y < MAP_RADIUS){

	} 
	else if (startLoc.x > MAP_RADIUS && startLoc.y > MAP_RADIUS){

	}
	else if (startLoc.x < MAP_RADIUS && startLoc.y < MAP_RADIUS){

	}
	else if (startLoc.x < MAP_RADIUS && startLoc.y > MAP_RADIUS){

	}
}

void Army::update(Scouting scoutClass){
	enemyBaseLocs = scoutClass.returnEnemyBaseLocs();
	
	for (int i = 0; i < zealots.size(); i++){
		zealots.at(i)->move();
	}
	if (zealots.size() > ZEALOT_RUSH_SIZE) {
		attack();
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

void Army::addZealot(BWAPI::Unit u){
	zealots.push_back(u);
}