#pragma once
#include "CombatUnits.h"

using namespace BWAPI;

CombatUnits::CombatUnits() { }

CombatUnits::~CombatUnits() { }

void CombatUnits::_init(){

}


// Fuck det her Hvornr skal der k�rer idle vs attack :O
void CombatUnits::update(){
	idleUpdate();
}


void CombatUnits::idleUpdate(){
	//Squad loops
	for (std::multimap<UnitType, Squad>::iterator it = unitMap.lower_bound(UnitTypes::Protoss_Dragoon); it != unitMap.upper_bound(UnitTypes::Protoss_Dragoon); it++){
		if (it->first == UnitTypes::Protoss_Dragoon){
			if (it->second.getUnitsInRadius(UnitTypes::Protoss_Dragoon.sightRange(), Filter::IsEnemy).size() > 0){
				dragoonMicro(&it->second);
			}
		}
	}
}

void CombatUnits::runAttack(Position attackPos){
	// Could be something from tactician? not very flexible.

	for (std::multimap<UnitType, Squad>::iterator it = unitMap.begin(); it != unitMap.end(); it++){
		if (it->first == UnitTypes::Protoss_Zealot){
			attackMovement(&it->second, attackPos);
		}
		if (it->first == UnitTypes::Protoss_Dragoon){
			if (it->second.getUnitsInRadius(UnitTypes::Protoss_Dragoon.sightRange(), Filter::IsEnemy).size() > 0){
				dragoonMicro(&it->second);
			}
			else {
				attackMovement(&it->second, attackPos);
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
		Color innerColor = unit->isAttacking() ? Colors::Green : Colors::Red;
		Color outerColor = unit->isAttackFrame() ? Colors::Green : Colors::Red;
		Broodwar->drawCircleMap(unit->getPosition(), 10, innerColor);
		Broodwar->drawCircleMap(unit->getPosition(), 12, outerColor);

		if (unit->isAttackFrame() || unit->getLastCommandFrame() >= Broodwar->getFrameCount()) {
			//Broodwar->sendText("Skipped a unit");
			continue;
		}
		if (unit->getGroundWeaponCooldown() == 0) {

			UnitCommand lastCommand = unit->getLastCommand();
			Unit targetUnit = getOptimalTarget(unit);
			if (!(lastCommand.getType() == UnitCommandTypes::Attack_Unit || targetUnit == lastCommand.getTarget())) {

				unit->attack(targetUnit);

				/*if (lastCommand.getTarget() == NULL) {
					Broodwar->drawCircleMap(lastCommand.getTargetPosition(), 12, Colors::Cyan);
					Broodwar->sendText("Last target not there %i, %i", lastCommand.getTargetPosition().x, lastCommand.getTargetPosition().y);
					continue;
				}
				int targetID = targetUnit->getID();
				int oldID = lastCommand.getTarget()->getID();

				Broodwar->sendText("Gave a new command to %i, old %i, new %i", unit->getID(),
					oldID, targetID);*/

			}
		}
		else {/*
			Broodwar->sendText("Already attacked (SHOULD NOT RIGHT NOW)");*/
			UnitCommand lastCommand(unit->getLastCommand());
			if (lastCommand.getType() != UnitCommandTypes::Move) {
				unit->move(escapePos(unit));
			}
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

void CombatUnits::attackMovement(Squad *squad, Position pos){
	squad->attack(pos);
}

void CombatUnits::addUnit(Unit u){
	idleMovement(u, rendezvousPos);
	saveUnitToSquad(u);
}

void CombatUnits::saveUnitToSquad(Unit u){
	if (unitMap.size() == 0 || unitMap.count(u->getType()) == 0){
		Broodwar->sendText("Squad count: %i - Unit added: %s", unitMap.count(u->getType()), u->getType().c_str());
		unitMap.insert(std::make_pair(u->getType(), Squad()));
	}
	if (unitMap.size() > 0){
		int tempSquadSize = 0;
		for (std::multimap<UnitType, Squad>::iterator it = unitMap.lower_bound(u->getType()); 
			it != unitMap.upper_bound(u->getType()); 
			it++){
			tempSquadSize = it->second.size();
			if (tempSquadSize < SQUAD_SIZE){
				it->second.insert(u);
				break;
			}
		}
		if (tempSquadSize == SQUAD_SIZE){
			Squad tempSq = Squad();
			tempSq.insert(u);
			unitMap.insert(std::make_pair(u->getType(), tempSq));
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
	for (std::multimap<UnitType, Squad>::iterator it = unitMap.lower_bound(unit->getType());
		it != unitMap.upper_bound(unit->getType()); 
		it++){

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
	return Position(Broodwar->self()->getStartLocation()) ;
	Unit enemy = unit->getClosestUnit(Filter::IsEnemy && Filter::CanAttack, unit->getType().sightRange());
	if (enemy != NULL){
		Position tempVec = (enemy->getPosition() - unit->getPosition()) * (-1);
		return unit->getPosition() - tempVec;
	}
	return unit->getPosition();
}

bool CombatUnits::enemyTooClose(Unit unit){
	Unit enemy = unit->getClosestUnit(Filter::IsEnemy);
	Broodwar->sendText("Fucker afstanden %i < %i", unit->getDistance(enemy), unit->getType().sightRange());
	return unit->getDistance(enemy) < unit->getType().sightRange()/*Magisk tal*/;
}


// Map vil ogs� v�re fint her.
int CombatUnits::getUnitCount(BWAPI::UnitType unitType) {
	int count = 0;
	for (std::multimap<UnitType, Squad>::iterator it = unitMap.lower_bound(unitType); it != unitMap.upper_bound(unitType); it++){
		count += it->second.size();
	}

	/*auto it = unitMap.lower_bound(unitType);
	if (unitMap.count(unitType) > 0){
		return (unitMap.count(unitType) - 1)*SQUAD_SIZE + it->second.size();
	}*/
	return count;
}

//Expand with oveloaded functions with larger flexibility for tactician.
void CombatUnits::setAttacking(){
	attacking = true;

}

void CombatUnits::setAnalyzed(Position pos){
	mapAnalyzed = true;
	rendezvousPos = pos;
}
