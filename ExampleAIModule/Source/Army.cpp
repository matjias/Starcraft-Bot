#include "Army.h"
#include "Constants.h"
#include "Scouting.h"
#include "Math.h"

using namespace BWAPI;


/*
Mby only work in unitsets instead of having all unit of a specfic type in a vector.
If unit doesn't belong to a specific squad, have it stored in an "overflow" squad.
*/

std::vector<Unit> zealotGens;
std::vector<Unit> dragoonGens;

std::vector<Unitset> zealotSquads;
std::vector<Unitset> dragoonSquads;

BWAPI::Position enemyBaseLocs;

Position idleLoc, enemyChoke, attackLoc;
bool mapAnalyzed, enemyBaseDest = false, zealotRush = false;
int moved, countAtEnemyChoke, zealotCount, attackNumber;
std::string fuckerString = "omg";

Army::Army(){}

Army::~Army(){}

void Army::_init(){
	mapAnalyzed = false;
	zealotSquads.push_back(Unitset());
	dragoonSquads.push_back(Unitset());
}

void Army::update(Scouting scoutClass){

	//Getting information from BWTA
	if (mapAnalyzed){
		idleLoc = BWTA::getNearestChokepoint(Position(Broodwar->self()->getStartLocation()))->getCenter();
		enemyChoke = BWTA::getNearestChokepoint(scoutClass.returnEnemyBaseLocs())->getCenter();
	}

	//Setting initial attack info
	enemyBaseLocs = scoutClass.returnEnemyBaseLocs();
	attackNumber = fmin(zealotSquads.size(), dragoonSquads.size());
	
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

	//Move to own region choke point, could probably be done in one function with squads and gens as arguments
	if (zealotSquads.size() > 0 && zealotGens.size() > 0){
		for (int i = 0; i < zealotSquads.size(); i++){
			if (zealotGens.at(i)->canAttackMove() && zealotGens.at(i)->isIdle() && !squadAtPos(zealotSquads.at(i), TilePosition(idleLoc))){
				zealotSquads.at(i).move(idleLoc);
			}
		}
	}
	if (dragoonSquads.size() > 0 && dragoonGens.size() > 0){
		for (int i = 0; i < dragoonSquads.size(); i++){
			if (dragoonGens.at(i)->canAttackMove() && dragoonGens.at(i)->isIdle() && !squadAtPos(dragoonSquads.at(i), TilePosition(idleLoc))){
				dragoonSquads.at(i).move(idleLoc);
			}
		}
	}
	

	//Attack
	if (zealotSquads.size() > 1 && dragoonSquads.size() > 1){
		attack(1);
	}
	/*
	Broodwar->drawTextScreen(300, 180, "%s", fuckerString.c_str());
	Broodwar->drawTextScreen(300, 200, "HejZ %i", zealotSquads.size());
	Broodwar->drawTextScreen(300, 220, " HejD %i",dragoonSquads.size());
	Broodwar->drawTextScreen(300, 240, " GensZ %i" ,zealotGens.size());
	Broodwar->drawTextScreen(300, 250, " genD %i",dragoonGens.size());
	*/
}

void Army::attack(int atNum){
	attackNumber = atNum;
	attack();
}


void Army::attack(Scouting scoutClass){
	enemyBaseLocs = scoutClass.returnEnemyBaseLocs();
	attack();
}

void Army::attack(){

	zealotRush();



	/*for (int i = 0; i < attackNumber; i++){
		if (squadAtPos(zealotSquads.at(i), TilePosition(enemyChoke))){
			zealotSquads.at(i).attack(attackLoc);
		}
		else if (zealotGens.at(i)->canAttackMove() && zealotGens.at(i)->isIdle() && !squadAtPos(zealotSquads.at(i), TilePosition(enemyChoke))){
			zealotSquads.at(i).attack(enemyChoke);
		}
		if (squadAtPos(dragoonSquads.at(i), TilePosition(enemyChoke))){
			dragoonSquads.at(i).attack(attackLoc);
		}
		else if (dragoonGens.at(i)->canAttackMove() && dragoonGens.at(i)->isIdle() && !squadAtPos(zealotSquads.at(i), TilePosition(enemyChoke))){
			dragoonSquads.at(i).attack(enemyChoke);
		}
	}*/
}

void Army::combat(){
	for (Unitset squad : dragoonSquads){
		for (Unit uSelf : squad){
			if (!uSelf->canAttack()){
				for (Unit unit : uSelf->getUnitsInRadius(5)){
					if (unit->getPlayer() != Broodwar->self()){
						int x = uSelf->getPosition().x + (uSelf->getPosition().x - unit->getPosition().x) * (-1 + (-1 / fabs(1.0*uSelf->getPosition().x - unit->getPosition().x)));
						int y = uSelf->getPosition().y + (uSelf->getPosition().y - unit->getPosition().y) * (-1 + (-1 / fabs(1.0*uSelf->getPosition().y - unit->getPosition().y)));
						uSelf->move(Position(x,y));
					}
				}
			}
		}
	}
}

void Army::zealotRush(){
	for (Unitset squad : zealotSquads){
		for (Unit u : squad){
			if (unitAtPos(u, TilePosition(enemyChoke)) && countAtEnemyChoke > ZEALOT_RUSH_SIZE){
				u->attack(attackLoc);
			}
			else if (u->canAttackMove() && u->isIdle() && !unitAtPos(u, TilePosition(enemyChoke))){
				u->move(enemyChoke);
			}
		}
	}
}

bool Army::buildZealot(BWAPI::Unit u){
	return u->train(UnitTypes::Protoss_Zealot);
}

bool Army::buildDragoon(BWAPI::Unit u){
	return u->train(UnitTypes::Protoss_Dragoon);
}

bool Army::buildObserver(BWAPI::Unit u){
	return u->train(UnitTypes::Protoss_Observer);
}

bool Army::buildCorsair(BWAPI::Unit u){
	return u->train(UnitTypes::Protoss_Corsair);
}

void Army::addZealot(BWAPI::Unit u){
	// Find a more intelligent solution for inserting right amount of
	if (zealotGens.size() == 0){
		zealotGens.push_back(u);
	}
	if (zealotSquads.size() > 0){
		for (int i = 0; i < zealotSquads.size(); i++){
			if (zealotSquads.at(i).size() < 4){
				zealotSquads.at(i).insert(u);
				break;
			}
			else if (i == zealotSquads.size() - 1){
				zealotSquads.push_back(Unitset());
				zealotSquads.at(i + 1).insert(u);
				zealotGens.push_back(u);
			}
		}
	}
}

void Army::addDragoon(BWAPI::Unit u){
	// Find a more intelligent solution for inserting right amount of unit in squad
	if (dragoonGens.size() == 0){
		dragoonGens.push_back(u);
	}
	if (dragoonSquads.size() > 0){
		for (int i = 0; i < dragoonSquads.size(); i++){
			if (dragoonSquads.at(i).size() < 4){
				dragoonSquads.at(i).insert(u);
				break;
			}
			else if (i == dragoonSquads.size() - 1){
				dragoonSquads.push_back(Unitset());
				dragoonSquads.at(i + 1).insert(u);
				dragoonGens.push_back(u);
			}
		}
	}
}


void Army::setAnalyzed(bool analyzed){
	mapAnalyzed = analyzed;
}

bool Army::squadAtPos(Unitset uSet, TilePosition pos){
	return TilePosition(uSet.getPosition()).x > pos.x - 6
		&& TilePosition(uSet.getPosition()).x < pos.x + 6
		&& TilePosition(uSet.getPosition()).y > pos.y - 6
		&& TilePosition(uSet.getPosition()).y < pos.y + 6;
}

bool Army::unitAtPos(Unit u, TilePosition pos){
	return u->getTilePosition().x > pos.x - 6
		&& u->getTilePosition().x < pos.x + 6
		&& u->getTilePosition().y > pos.y - 6
		&& u->getTilePosition().y < pos.y + 6;
}


void Army::enemyBaseDestroyed(){
	enemyBaseDest = true;
}
/*void Army::addSquadMember(Unit u){
	//UnitType uType = u->getType();
	switch (u->getType())
	{
	case UnitTypes::Protoss_Zealot: addZealot(u);
		break;
	default:
		break;
	}


	if (!squads.empty()){
		if (!squads.at(i).empty()){
			squads.at(i).insert(u);
		}
	}
}*/

