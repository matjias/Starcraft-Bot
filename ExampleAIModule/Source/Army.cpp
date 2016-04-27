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
bool mapAnalyzed, enemyBaseDest = false, zealotRush = false, rushInitiated = false;
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

	//Debugging
	//debugDraw(dragoonSquads, dragoonGens);
	//debugDraw(zealotSquads, zealotGens);

	//Move to own region choke point
	idleMovement(zealotSquads, zealotGens);
	idleMovement(dragoonSquads, dragoonGens);

	//zealotRush();

	//Attack
	if (zealotSquads.size() > 1 && dragoonSquads.size() > 1){
		attack(1);
	}
	else if (Broodwar->enemy()->getRace() == Races::Protoss){
		zealotRush();
	}
	/*
	Broodwar->drawTextScreen(300, 180, "%s", fuckerString.c_str());
	Broodwar->drawTextScreen(300, 200, "HejZ %i", zealotSquads.size());
	Broodwar->drawTextScreen(300, 220, " HejD %i",dragoonSquads.size());
	Broodwar->drawTextScreen(300, 240, " GensZ %i" ,zealotGens.size());
	Broodwar->drawTextScreen(300, 250, " genD %i",dragoonGens.size());
	*/
}

void Army::idleMovement(std::vector<Unitset> squads, std::vector<Unit> gens){
	if (squads.size() > 0 && gens.size() > 0){
		for (int i = 0; i < squads.size(); i++){
			if (gens.at(i)->canAttackMove() && gens.at(i)->isIdle() && !squadAtPos(squads.at(i), TilePosition(idleLoc))){
				squads.at(i).attack(idleLoc);
			}
		}
	}
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
	attackMovement(zealotSquads, zealotGens);
	attackMovement(dragoonSquads, dragoonGens);
	combat();
}

void Army::attackMovement(std::vector<Unitset> squads, std::vector<Unit> gens){
	for (int i = 0; i < attackNumber; i++){
		if (squadAtPos(squads.at(i), TilePosition(enemyChoke))){
			squads.at(i).attack(attackLoc);
		}
		else if (gens.at(i)->canAttackMove() && gens.at(i)->isIdle() && !squadAtPos(squads.at(i), TilePosition(enemyChoke))){
			squads.at(i).attack(enemyChoke);
		}
	}
}

void Army::combat(){
	for (Unitset squad : dragoonSquads){
		for (Unit uSelf : squad){
			if (!uSelf->canAttack()){
				for (Unit unit : uSelf->getUnitsInRadius(200)){
					uSelf->attack(escapePos(uSelf));
					break;
				}
			}
		}
	}
}

void Army::zealotRush(){
	for (int i = 0; i < zealotSquads.size(); i++){
		if (!squadAtPos(zealotSquads.at(i), TilePosition(enemyChoke)) && zealotCount > ZEALOT_RUSH_SIZE && !rushInitiated){
			zealotGens.at(i)->attack(enemyChoke);
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
	zealotCount++;
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
/*
BWTA::Chokepoint getNextChokePoint(){

}
*/


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



void Army::debugDraw(std::vector<Unitset> squads, std::vector<Unit> gens){
	if (squads.size() > 0 && gens.size() > 0){
		for (int i = 0; i < squads.size(); i++){
			for (Unit unit : squads.at(i)){
				if (unit != NULL){
					Unitset enemies = unit->getUnitsInRadius(unit->getType().sightRange(), Filter::IsEnemy && Filter::CanAttack);

					for (auto &enemy : enemies){

						Color colorToDraw = enemy->getType().isWorker() ? Colors::Green : Colors::Red;
						Broodwar->drawCircleMap(enemy->getPosition(),
							enemy->getType().groundWeapon().maxRange(),
							colorToDraw);

						Position startPos = unit->getPosition();
						Position endPos = escapePos(unit);
						Broodwar->drawLineMap(startPos, endPos, Colors::Orange);
						break;
					}
				}
			}
		}
	}
}


Position Army::escapePos(Unit unit){
	Unit enemy = unit->getClosestUnit(Filter::IsEnemy && Filter::CanAttack, unit->getType().sightRange());
	if (enemy != NULL){
		int x = unit->getPosition().x + (unit->getPosition().x - enemy->getPosition().x) * (-1 + (-1 / fabs(1.0*unit->getPosition().x - enemy->getPosition().x)));
		int y = unit->getPosition().y + (unit->getPosition().y - enemy->getPosition().y) * (-1 + (-1 / fabs(1.0*unit->getPosition().y - enemy->getPosition().y)));
		return Position(x, y);
	}
	return unit->getPosition();
}