#pragma once
#include "CombatUnits.h"

using namespace BWAPI;

CombatUnits::CombatUnits() { }

CombatUnits::~CombatUnits() { }

void CombatUnits::_init(){

}


// Fuck det her Hvornr skal der k�rer idle vs attack :O
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
	
}

void CombatUnits::runAttack(){
	// Could be something from tactician? not very flexible.

	for (std::multimap<UnitType, Squad>::iterator it = unitMap.begin(); it != unitMap.end(); it++){
		if (it->first == UnitTypes::Protoss_Zealot){
			attackMovement(&it->second);
		}
		if (it->first == UnitTypes::Protoss_Dragoon){
			if (it->second.getUnitsInRadius(UnitTypes::Protoss_Dragoon.sightRange(), Filter::IsEnemy).size() > 0){
				dragoonMicro(&it->second);
			}
			else{
				attackMovement(&it->second);
			}
		}
	}

	//for (int i = 0; i < squadCount; i++){
	//	// fuk den her kode
	//	if (dragoonSquads.size() > i){
	//		if (dragoonSquads.at(i).getUnitsInRadius(UnitTypes::Protoss_Dragoon.sightRange(), Filter::IsEnemy).size() > 0){
	//			dragoonMicro(dragoonSquads.at(i));
	//		}
	//		else{
	//			attackMovement(&dragoonSquads.at(i));
	//		}
	//	}
	//	if (zealotSquads.size() > i){
	//	}
	//}
}


void CombatUnits::dragoonMicro(Squad * squad){
	for (auto &unit : *squad){
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
	idleMovement(u, ownChoke);
	saveUnitToSquad(u);
}

void CombatUnits::saveUnitToSquad(Unit u){

	if (unitMap.size() == 0 || unitMap.count(u->getType()) == 0){
		unitMap.insert(std::make_pair(u->getType(), Squad()));
	}
	if (unitMap.size() > 0){
		for (auto &squad : unitMap){
			if (squad.first == u->getType()){
				if (squad.second.size() < SQUAD_SIZE){
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

bool CombatUnits::deleteUnit(Unit unit){

	for (std::multimap<UnitType, Squad>::iterator it = unitMap.lower_bound(unit->getType()); it != unitMap.upper_bound(unit->getType()); it++){
		if (it->second.contains(unit)){
			return it->second.erase(unit);
		}
	}

	return false;
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


// Map vil ogs� v�re fint her.
int CombatUnits::getUnitCount(BWAPI::UnitType unitType) {
	auto it = unitMap.upper_bound(unitType);
	it--;
	if (unitMap.count(unitType) > 0){
		return (unitMap.count(unitType) - 1)*SQUAD_SIZE + it->second.size();
	}
	return 0;
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
