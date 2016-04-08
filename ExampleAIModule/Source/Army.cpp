#include "Army.h"
#include "Constants.h"
#include "Scouting.h"

using namespace BWAPI;

std::vector<Unit> zealots; // Dead Zealots should be removed from the vector!
std::vector<Unit> dragoons;

/*
Mby only work in unitsets instead of having all unit of a specfic type in a vector.
If unit doesn't belong to a specific squad, have it stored in a "overflow" squad.
*/

std::vector<Unitset> squads;

BWAPI::Position enemyBaseLocs;

Position idleLoc, enemyChoke, attackLoc;
bool mapAnalyzed, enemyBaseDest = false, zealotRush = false;
int moved = 0;
int countAtEnemyChoke = 0;

Army::Army(){}

Army::~Army(){}

void Army::_init(){
	mapAnalyzed = false;
	squads.push_back(Unitset());
}

void Army::update(Scouting scoutClass){
	if (mapAnalyzed){
		idleLoc = BWTA::getNearestChokepoint(Position(Broodwar->self()->getStartLocation()))->getCenter();
		enemyChoke = BWTA::getNearestChokepoint(scoutClass.returnEnemyBaseLocs())->getCenter();
	}
	enemyBaseLocs = scoutClass.returnEnemyBaseLocs();
	
	if (!enemyBaseDest){
		attackLoc = enemyBaseLocs;
	}
	else{
		for (std::map<TilePosition, Scouting::BuildingStruct*, Scouting::CustomMapCompare>::iterator iterator = scoutClass.getEnemyStructures().begin();
			iterator != scoutClass.getEnemyStructures().end(); iterator++) {
			attackLoc = Position(iterator->first);
			break;
		}
	}

	if (zealots.size() > ZEALOT_RUSH_SIZE) {
		attack();
	}
	else {
		for (Unit u : zealots){
			if (u->canAttackMove() && u->isIdle() && !zealotAtPos(u, TilePosition(idleLoc))){
				u->move(idleLoc);
				moved++;
			}

		}
	}


}
void Army::attack(Scouting scoutClass){
	enemyBaseLocs = scoutClass.returnEnemyBaseLocs();
	attack();
}

void Army::attack(){
	countAtEnemyChoke = 0;
	for (Unit u : zealots){
		if (u->canAttackMove() && u->isIdle() && zealotAtPos(u, TilePosition(enemyChoke))){
			countAtEnemyChoke++;
		}
	}

	//if (!zealotRush()){
		
	//}
	//else
	//{
	//	zealotRush();
	//}

}


void Army::zealotRush(){
	for (Unit u : zealots){
		if (zealotAtPos(u, TilePosition(enemyChoke)) && countAtEnemyChoke > ZEALOT_RUSH_SIZE){
			u->attack(attackLoc);
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

bool Army::buildDragoon(BWAPI::Unit u){
	return u->train(UnitTypes::Protoss_Dragoon);
}

void Army::addZealot(BWAPI::Unit u){
	zealots.push_back(u);
	// Find a more intelligent solution for inserting right amount of
	for (Unitset squad : squads){
		if (squad.size() <= 7){
			squad.insert(u);
		}
	}
}

void Army::addDragoon(BWAPI::Unit u){
	dragoons.push_back(u);
	// Find a more intelligent solution for inserting right amount of unit in squad
	for (Unitset squad : squads){
		if (squad.size() <= 7){
			squad.insert(u);
		}
	}
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


void Army::enemyBaseDestroyed(){
	enemyBaseDest = true;
}

void Army::addSquadMember(int i, Unit u){
	if (!squads.empty()){
		if (!squads.at(i).empty()){
			squads.at(i).insert(u);
		}
	}
}

void Army::buildSquad(std::vector<std::pair<BWAPI::UnitType, int>> list){
	
}