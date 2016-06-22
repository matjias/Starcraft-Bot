#pragma once
#include "CombatUnits.h"
#include "Constants.h"

using namespace BWAPI;

CombatUnits::CombatUnits() { }

CombatUnits::~CombatUnits() { }

void CombatUnits::_init(){

}

// Fuck det her Hvornr skal der kører idle vs attack :O
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
	for (std::multimap<UnitType, Squad>::iterator it = unitMap.lower_bound(UnitTypes::Protoss_Observer); it != unitMap.upper_bound(UnitTypes::Protoss_Observer); it++){
		
		if (it->first == UnitTypes::Protoss_Observer){
			Unitset enemyU = Broodwar->enemy()->getUnits();

			Position targ;
			for (auto &e : enemyU) {
				if (e->isCloaked() || e->isBurrowed()) {
					targ = e->getPosition();
					break;
				}
			}

			if (targ != Position(0, 0)) {
				idleMovement(&it->second, targ);
			}
			else {
				Unit closestDragoon = it->second.getClosestUnit(Filter::GetType ==
					UnitTypes::Protoss_Dragoon);
				Unit closestZealot = it->second.getClosestUnit(Filter::GetType ==
					UnitTypes::Protoss_Zealot);

				if (closestDragoon) {
					idleMovement(&it->second, closestDragoon->getPosition());
				}
				else if (closestZealot) {
					idleMovement(&it->second, closestZealot->getPosition());
				}

				
			}
		}
	}
	for (std::multimap<UnitType, Squad>::iterator it = unitMap.lower_bound(UnitTypes::Protoss_Probe); it != unitMap.upper_bound(UnitTypes::Protoss_Probe); it++){
		if (it->first == UnitTypes::Protoss_Probe){
			it->second.attack(it->second.getClosestUnit(Filter::IsEnemy));
		}

	}
}

void CombatUnits::runAttack(Position attackPos){
	// Could be something from tactician? not very flexible.

	for (std::multimap<UnitType, Squad>::iterator it = unitMap.begin(); it != unitMap.end(); it++){
		if (it->first == UnitTypes::Protoss_Observer) {
			continue;
		}
		
		/*if (it->second.isIdle() || attackPos != lastAttackPos){
			attackMovement(&it->second, attackPos);
		}*/
		attackMovement(&it->second, attackPos);
		
		/*if (it->first == UnitTypes::Protoss_Zealot && it->second.){
			attackMovement(&it->second, attackPos);
		}
		if (it->first == UnitTypes::Protoss_Dragoon){
			dragoonMicro(&it->second);
			if (it->second.getUnitsInRadius(UnitTypes::Protoss_Dragoon.sightRange(), Filter::IsEnemy).size() == 0){
				attackMovement(&it->second, attackPos);
			}
		}*/

	}
	lastAttackPos = attackPos;

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

void CombatUnits::retreat(Position retreatPos){
	for (std::multimap<UnitType, Squad>::iterator it = unitMap.begin(); it != unitMap.end(); it++) {
		if (it->first == UnitTypes::Protoss_Observer) {
			continue;
		}

		if (it->second.getPosition().getDistance(retreatPos) > RETREAT_DISTANCE) {
			/*if (it->second.isIdle() || retreatPos != lastAttackPos) {
				moveCommand(&it->second, retreatPos);
				lastAttackPos = retreatPos;
			}*/
			moveCommand(&it->second, retreatPos);
			lastAttackPos = retreatPos;
		}
	}
}

void CombatUnits::dragoonMicro(Squad * squad){
	Unit target = squad->getClosestUnit(Filter::IsEnemy);

	if (!target->exists()) {
		squad->move(rendezvousPos);
	}

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
			//Unit targetUnit = getOptimalTarget(unit);
			if (!(lastCommand.getType() == UnitCommandTypes::Attack_Unit || target == lastCommand.getTarget())) {

				unit->attack(target);

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
			if (unit->getUnitsInWeaponRange(UnitTypes::Protoss_Dragoon.groundWeapon(), Filter::IsEnemy).size() > 0) {
				UnitCommand lastCommand(unit->getLastCommand());
				if (lastCommand.getType() != UnitCommandTypes::Move) {
					unit->move(escapePos(unit));
				}
			}
		}
	}
}

Unit CombatUnits::extractUnit(UnitType unitType){
	//Broodwar->sendText("Prøver at fjerne %s fra combatUnits", unitType.c_str());

	Unit tempProbe = NULL;
	Squad *squad = &unitMap.lower_bound(unitType)->second;
	for (auto& probe : *squad){
		tempProbe = probe;
		break;
	}
	squad->erase(tempProbe);
	return tempProbe;
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

void CombatUnits::moveCommand(Squad *squad, Position pos){
	squad->move(pos);
}

void CombatUnits::addUnit(Unit u){
	idleMovement(u, rendezvousPos);
	saveUnitToSquad(u);
}

void CombatUnits::saveUnitToSquad(Unit u){
	if (unitMap.size() == 0 || unitMap.count(u->getType()) == 0){
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
	return unit->getDistance(enemy) < unit->getType().sightRange()/*Magisk tal*/;
}


// Map vil også være fint her.
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
