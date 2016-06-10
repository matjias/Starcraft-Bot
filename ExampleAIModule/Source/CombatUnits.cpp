#pragma once
#include "CombatUnits.h"

using namespace BWAPI;

CombatUnits::CombatUnits() { }

CombatUnits::~CombatUnits() { }

void CombatUnits::_init(){

}


// Fuck det her Hvornr skal der kører idle vs attack :O
void CombatUnits::update(){

}


void CombatUnits::idleUpdate(){
	
	//Squad loops
	for (int i = 0; i < zealotSquads.size(); i++){
		idleMovement(&zealotSquads.at(i), ownChoke);

		//Zealot Rush
		if (zealotCount > ZEALOT_RUSH_COUNT){
			attackMovement(&zealotSquads.at(i));
		}
	}
	for (int i = 0; i < dragoonSquads.size(); i++){
		if (attacking){
			dragoonMicro(dragoonSquads.at(i));
		}
	}
}

void CombatUnits::attackUpdate(){
	// Someding
}

void CombatUnits::dragoonMicro(Squad squad){
	for (auto& unit : squad){
		if (unit->isUnderAttack()){
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

void CombatUnits::attackMovement(Squad *squad){
	squad->attack(attackLoc);
}

void CombatUnits::addUnit(Unit u){
	units.push_back(u);
	
	if (u->getType() == UnitTypes::Protoss_Zealot && u->getPlayer() == Broodwar->self()){
		zealotCount++;
		saveUnitToSquad(u, &zealotSquads);
	}
	else if (u->getType() == UnitTypes::Protoss_Dragoon && u->getPlayer() == Broodwar->self()){
		saveUnitToSquad(u, &dragoonSquads);
	}
}

void CombatUnits::saveUnitToSquad(Unit u, std::vector<Squad> *squads){
	if (squads->size() == 0){
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
	}
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
	if (squads.size() > 0){
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

Position CombatUnits::escapePos(Unit unit){
	Unit enemy = unit->getClosestUnit(Filter::IsEnemy && Filter::CanAttack, unit->getType().sightRange());
	if (enemy != NULL){
		int x = unit->getPosition().x + (unit->getPosition().x - enemy->getPosition().x) * (-1 + (-1 / fabs(1.0*unit->getPosition().x - enemy->getPosition().x)));
		int y = unit->getPosition().y + (unit->getPosition().y - enemy->getPosition().y) * (-1 + (-1 / fabs(1.0*unit->getPosition().y - enemy->getPosition().y)));
		return Position(x, y);
	}
	return unit->getPosition();
}

int CombatUnits::getUnitCount(BWAPI::UnitType unitType) {
	int number = 0;
	
	for (int i; i < units.size(); i++) {
		if (units[i]->getType() == unitType) {
			number++;
		}
	}

	return number;
}
