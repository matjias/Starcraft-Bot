#pragma once
#include "CombatUnits.h"

using namespace BWAPI;

CombatUnits::CombatUnits() { }

CombatUnits::~CombatUnits() { }

void CombatUnits::_init(){

}


// Fuck det her Hvornr skal der kører idle vs attack :O
void CombatUnits::update(){
	if (attacking){
		runAttack();
	}
	else{
		//idleUpdate();
	}
}


void CombatUnits::idleUpdate(){
	//Squad loops
	for (int i = 0; i < zealotSquads.size(); i++){
		idleMovement(&zealotSquads.at(i), ownChoke);
	}
	for (int i = 0; i < dragoonSquads.size(); i++){
		idleMovement(&dragoonSquads.at(i), ownChoke);
	}
}

void CombatUnits::runAttack(){
	// Could be something from tactician? not very flexible.
	int squadCount = std::min(zealotSquads.size(), dragoonSquads.size());

	for (int i = 0; i < squadCount; i++){
		/*if (zealotCount > ZEALOT_RUSH_COUNT){
			attackMovement(&zealotSquads.at(i));
		}*/
		// fuk den her kode
		if (dragoonSquads.size() > i){
			if (dragoonSquads.at(i).getUnitsInRadius(UnitTypes::Protoss_Dragoon.sightRange(), Filter::IsEnemy).size() > 0){
				dragoonMicro(dragoonSquads.at(i));
			}
			else{
				attackMovement(&dragoonSquads.at(i));
			}
		}
		if (zealotSquads.size() > i){
			attackMovement(&zealotSquads.at(i));
		}
	}
}


void CombatUnits::dragoonMicro(Squad squad){
	for (auto& unit : squad){
		if (unit->getHitPoints() < (unit->getType().maxHitPoints() / 2) || enemyTooClose(unit)){
			if (unit->canAttack()){
				unit->attack(getOptimalTarget(unit));
			}
			unit->move(escapePos(unit));
		}
		else {
			unit->attack(getOptimalTarget(unit));
		}
	}
}

Unit CombatUnits::getOptimalTarget(Unit unit){
	// implement the ultimate logic for attacking the right oppponent
	return unit->getClosestUnit(Filter::IsEnemy);
}

void CombatUnits::idleMovement(Squad *squad, Position idleLoc){
	if (!squadAtPos(*squad, TilePosition(idleLoc))){
		squad->attack(idleLoc);
	}
}

void CombatUnits::idleMovement(Unit u, Position idleLoc){
	if (!unitAtPos(u, TilePosition(idleLoc))){
		u->attack(idleLoc);
	}
}

void CombatUnits::attackMovement(Squad *squad){
	squad->attack(attackLoc);
}

void CombatUnits::addUnit(Unit u){
	units.push_back(u);
	idleMovement(u, ownChoke);
	
	if (u->getType() == UnitTypes::Protoss_Zealot && u->getPlayer() == Broodwar->self()){
		zealotCount++;
		saveUnitToSquad(u, &zealotSquads);
	}
	else if (u->getType() == UnitTypes::Protoss_Dragoon && u->getPlayer() == Broodwar->self()){
		saveUnitToSquad(u, &dragoonSquads);
	}
}

void CombatUnits::saveUnitToSquad(Unit u, std::vector<Squad> *squads){

	if (unitMap.size() == 0){
		unitMap.insert(std::make_pair(u->getType(), Squad()));
	}
	if (unitMap.size() > 0){
		for (auto &squad : unitMap){
			if (squad.first == u->getType()){
				if (squad.second.size() <= SQUAD_SIZE){
					squad.second.insert(u);
					break;
				}
				else{
					Squad tempSq = Squad();
					tempSq.insert(u);
					unitMap.insert(std::make_pair(u->getType(), tempSq));
				}
			}
		}
	}

	/*if (squads->size() == 0){
		squads->push_back(Squad());
	}
	if (squads->size() > 0){
		for (int i = 0; i < squads->size(); i++){
			if (squads->at(i).size() <= SQUAD_SIZE){
				squads->at(i).insert(u);
				break;
			}
			else if (i == squads->size() - 1){
				squads->push_back(Squad());
				squads->at(i + 1).insert(u);
			}
		}
	}*/
}

bool CombatUnits::squadAtPos(Squad uSet, TilePosition pos){
	return TilePosition(uSet.getPosition()).x > pos.x - POS_RADIUS
		&& TilePosition(uSet.getPosition()).x < pos.x + POS_RADIUS
		&& TilePosition(uSet.getPosition()).y > pos.y - POS_RADIUS
		&& TilePosition(uSet.getPosition()).y < pos.y + POS_RADIUS;
}

bool CombatUnits::unitAtPos(Unit u, TilePosition pos){
	return u->getTilePosition().x > pos.x - POS_RADIUS
		&& u->getTilePosition().x < pos.x + POS_RADIUS
		&& u->getTilePosition().y > pos.y - POS_RADIUS
		&& u->getTilePosition().y < pos.y + POS_RADIUS;
}

void CombatUnits::debugDraw(std::vector<Squad> squads){
	/*if (squads.size() > 0){
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
	}*/
}

Position CombatUnits::escapePos(Unit unit){
	Unit enemy = unit->getClosestUnit(Filter::IsEnemy && Filter::CanAttack, unit->getType().sightRange());
	if (enemy != NULL){
		int x = unit->getPosition().x + (unit->getPosition().x - enemy->getPosition().x) * (-1 + (-1 / fabs(1.0*unit->getPosition().x - enemy->getPosition().x)));
		int y = unit->getPosition().y + (unit->getPosition().y - enemy->getPosition().y) * (-1 + (-1 / fabs(1.0*unit->getPosition().y - enemy->getPosition().y)));
		return Position(x, y);
	}
	return unit->getPosition();
}

bool CombatUnits::enemyTooClose(Unit unit){
	Unit enemy = unit->getClosestUnit(Filter::IsEnemy);
	return unit->getDistance(enemy) < unit->getType().groundWeapon().maxRange() * 2/*Magisk tal*/;
}


// Map vil også være fint her.
int CombatUnits::getUnitCount(BWAPI::UnitType unitType) {
	int number = 0;

	if (unitType == UnitTypes::Protoss_Zealot){
		number = zealotCount;
	}

	/*for (int i; i < units.size(); i++) {
		if (units[i]->getType() == unitType) {
			number++;
		}
	}*/

	return number;
}

//Expand with oveloaded functions with larger flexibility for tactician.
void CombatUnits::setAttacking(Position pos){
	attacking = true;
	attackLoc = pos;
	enemyChoke = BWTA::getNearestChokepoint(attackLoc)->getCenter();
}

void CombatUnits::setAnalyzed(){
	mapAnalyzed = true;
	ownChoke = BWTA::getNearestChokepoint(Broodwar->self()->getStartLocation())->getCenter();
}
