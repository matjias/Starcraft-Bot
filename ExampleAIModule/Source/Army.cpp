#include "Army.h"
#include "Scouting.h"

using namespace BWAPI;

std::vector<Unit>zealots;

BWAPI::Position enemyBaseLocs;

const int ZEALOT_RUSH_SIZE = 18;

Army::Army(){}

Army::~Army(){}


void Army::_init(){


}

void run(Scouting scoutClass){
	enemyBaseLocs = scoutClass.returnEnemyBaseLocs();

	attack();
}

void attack(){
	for (Unit u : zealots){
		if (u->isIdle() && zealots.size() >= ZEALOT_RUSH_SIZE/* + gatewayCount*/){
			u->attack(enemyBaseLocs);
		}
	}
}

bool buildZealot(BWAPI::Unit u){
	return u->train(UnitTypes::Protoss_Zealot);
}

void addZealot(BWAPI::Unit u){
	zealots.push_back(u);
}