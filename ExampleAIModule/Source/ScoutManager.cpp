#pragma once
#include "ScoutManager.h"
#include "StrategyDecider.h"

// Most of our data types are either ScoutManager or BWAPI
using namespace BWAPI;

ScoutManager::ScoutManager() { }

ScoutManager::~ScoutManager() { }

// Following function should ONLY ever 
// be called from the Unit Tests
void ScoutManager::setBroodwarMock(Game* broodwarPtr) {
	BroodwarPtr = broodwarPtr;
}

// Quick testing function for testing override of the
// Broodwar keyword (above function has already been called)
Player ScoutManager::testBroodwarReturnPlayer() {
	return Broodwar->self();
}

bool ScoutManager::_init() {
	// If we have already defined called _init once 
	// and defined the spawns, then we just stop
	if (spawns.size() > 0) {
		return false;
	}

	TilePosition::list unsortedSpawns;
	TilePosition ownSpawn = Broodwar->self()->getStartLocation();

	// Adds all the spawns to a list, excluding our own spawn
	for (auto &location : Broodwar->getStartLocations()) {
		if (location != ownSpawn) {
			unsortedSpawns.push_back(location);
		}
	}

	// Edge case of it being a 1v1 map
	if (unsortedSpawns.size() == 1) {
		foundEnemyBaseInit(unsortedSpawns.at(0));
	}

	// Presorts the start locations so the shortest spawn
	// to our spawn is first in the list, and our spawn is
	// Uses insertion sort, but the size of spawn locations
	// is small enough for this to not be an issue
	for (u_int i = 1; i < unsortedSpawns.size(); i++) {
		for (u_int j = i; j > 0; j--) {
			if (unsortedSpawns.at(j).getApproxDistance(ownSpawn) <
				unsortedSpawns.at(j - 1).getApproxDistance(ownSpawn)) {

				std::swap(unsortedSpawns.at(j), unsortedSpawns.at(j - 1));
			}
		}
	}

	// Now we add them to the list of spawns
	for (auto &loc : unsortedSpawns) {
		LocationStruct *locStruct = new LocationStruct();
		locStruct->location = loc;

		spawns.push_back(locStruct);
	}

	return true;
}

void ScoutManager::recordUnitDiscover(Unit u) {
	// Do we already know about this unit?
	if (enemyUnits.count(u->getID()) == 1) {
		// Then we update their position and when we last saw it
		enemyUnits.at(u->getID())->lastKnownPosition = u->getPosition();
		enemyUnits.at(u->getID())->lastScouted = Broodwar->getFrameCount();
	}
	// Otherwise we need to record it
	else {
		knownEnemyValue += u->getType().mineralPrice() + u->getType().gasPrice()*GAS_TO_MINERALS;
		UnitStruct *uStruct = new UnitStruct();
		uStruct->unit = u;
		uStruct->lastKnownPosition = u->getPosition();
		uStruct->lastScouted = Broodwar->getFrameCount();

		enemyUnits.insert(std::make_pair(u->getID(), uStruct));

		// Increment enemyUnitsAmount
		if (enemyUnitsAmount.count(u->getType()) > 0) {
			enemyUnitsAmount.at(u->getType()) += 1;
		}
		else {
			enemyUnitsAmount.insert(std::make_pair(u->getType(), 1));
		}

		// Tell StrategyDecider about a new unit
		strategyDeciderPtr->updateStrategy();
	}

	// Was it the enemy nexus?
	if (u->getType().isResourceDepot() && !foundEnemySpawn) {
		foundEnemyBase(TilePosition(u->getPosition()));
	}
}

int ScoutManager::recordUnitDestroy(Unit u) {
	int elementsRemoved = enemyUnits.erase(u->getID());
	knownEnemyValue -= u->getType().mineralPrice() + u->getType().gasPrice()*GAS_TO_MINERALS;

	// Decrement enemyUnitsAmount
	if (enemyUnitsAmount.at(u->getType()) == 1) {
		enemyUnitsAmount.erase(u->getType());
	}
	else {
		enemyUnitsAmount.at(u->getType()) -= 1;
	}

	// Something went wrong if we destroyed a unit
	// we did not have a record of
	if (elementsRemoved == 0) {
		// TODO: Currently just tells the player about it,
		// but this should be covered somewhere
		Broodwar->sendText("A unit was destroyed which was not recorded");
	}

	return elementsRemoved;
}

void ScoutManager::recordUnitEvade(Unit u) {
	if (enemyUnits.count(u->getID()) == 1) {
		enemyUnits.at(u->getID())->lastKnownPosition = u->getPosition();
		enemyUnits.at(u->getID())->lastScouted = Broodwar->getFrameCount();
	}
	else {
		// Something went wrong, for now just tell the client
		Broodwar->sendText("A unit has evaded that wasnt recorded");
	}
}

int ScoutManager::getAmountOfEnemyUnit(UnitType u) {
	if (enemyUnitsAmount.count(u) == 1) {
		// Return amount cached
		return enemyUnitsAmount.at(u);
	}
	else {
		// No units with this unit type cached
		return 0;
	}
}

std::map<BWAPI::UnitType, int> ScoutManager::getEnemyUnitsAmount() {
	return enemyUnitsAmount;
}

int ScoutManager::getEnemyBaseCount() {
	if (Broodwar->enemy()->getRace() == Races::Protoss) {
		return getAmountOfEnemyUnit(UnitTypes::Protoss_Nexus);
	}
	else if (Broodwar->enemy()->getRace() == Races::Terran) {
		return getAmountOfEnemyUnit(UnitTypes::Terran_Command_Center);
	}
	else if (Broodwar->enemy()->getRace() == Races::Zerg) {
		return getAmountOfEnemyUnit(UnitTypes::Zerg_Hatchery) + 
			getAmountOfEnemyUnit(UnitTypes::Zerg_Lair) + 
			getAmountOfEnemyUnit(UnitTypes::Zerg_Hive);
	}
	return 0;
}

int ScoutManager::getEnemyWorkerCount() {
	if (Broodwar->enemy()->getRace() == Races::Protoss) {
		return getAmountOfEnemyUnit(UnitTypes::Protoss_Probe);
	}
	else if (Broodwar->enemy()->getRace() == Races::Terran) {
		return getAmountOfEnemyUnit(UnitTypes::Terran_SCV);
	}
	else if (Broodwar->enemy()->getRace() == Races::Zerg) {
		return getAmountOfEnemyUnit(UnitTypes::Zerg_Drone);
	}
	return 0;
}

int ScoutManager::getEnemyDefenseValue() {
	if (Broodwar->enemy()->getRace() == Races::Protoss) {
		return getAmountOfEnemyUnit(UnitTypes::Protoss_Photon_Cannon) * 
			UnitTypes::Protoss_Photon_Cannon.mineralPrice();
	}
	else if (Broodwar->enemy()->getRace() == Races::Terran) {
		return getAmountOfEnemyUnit(UnitTypes::Terran_Bunker) * 
			(UnitTypes::Terran_Bunker.mineralPrice() + 
			UnitTypes::Terran_Marine.mineralPrice() * 2) + 
			getAmountOfEnemyUnit(UnitTypes::Terran_Missile_Turret) *
			(UnitTypes::Terran_Missile_Turret.mineralPrice());
	}
	else if (Broodwar->enemy()->getRace() == Races::Zerg) {
		return getAmountOfEnemyUnit(UnitTypes::Zerg_Creep_Colony) *
			UnitTypes::Zerg_Creep_Colony.mineralPrice() +
			getAmountOfEnemyUnit(UnitTypes::Zerg_Sunken_Colony) *
			UnitTypes::Zerg_Sunken_Colony.mineralPrice() +
			getAmountOfEnemyUnit(UnitTypes::Zerg_Spore_Colony) *
			UnitTypes::Zerg_Spore_Colony.mineralPrice();
	}
	return 0;
}

float ScoutManager::getEnemyArmyValue() {
	// @TODO: Get army value of enemy combat units, 1 gas = 1 GAS_TO_MINERALS
	return knownEnemyValue;
}

void ScoutManager::updateScoutManager() {
	if (!isScouting) {
		return;
	}
	
	if (!foundEnemySpawn) {
		findEnemySpawn();
	}
	else if (peekEnemySpawn) {
		scoutPeekEnemySpawn();
	}
	else if (peekEnemyExpansions) {
		// See if enemy is expanding anywhere

	}
	else {
		// Scout is doing nothing and should do nothing
	}
}

void ScoutManager::findEnemySpawn() {
	// @TODO:	Should probably break this one up into smaller functions..
	//			rather big as it is right now

	std::vector<ScoutAndGoalStruct*> scouts = scoutUnitsPtr->getScouts();
	bool hasToUpdateList = false;

	// Now we update if any of our scouts see their target
	for (auto &scoutAndGoal : scouts) {

		if (scoutAndGoal->goal == Position(0, 0)) {
			continue;
		}

		Position goalPos = scoutAndGoal->goal;
		TilePosition goalTilePos = TilePosition(goalPos);
		/*Position scoutPos = scoutAndGoal->scout->getPosition();

		int radius = scoutAndGoal->scout->getType().sightRange();
		int xDif = goalPos.x - scoutPos.x;
		int yDif = goalPos.y - scoutPos.y;*/

		if (/*xDif * xDif + yDif * yDif < radius * radius &&*/
			Broodwar->isVisible(goalTilePos)) {

			for (auto &spawn : spawns) {
				if (goalTilePos == spawn->location) {
					spawn->scouted = true;
					scoutAndGoal->reachedGoal = true;

					hasToUpdateList = true;
					break;
				}
			}
		}
	}

	// Check to make sure all scouts have a locaton to go to
	for (auto &scoutAndGoal : scouts) {
		// Do we have a scout that has not been assigned a goal yet?
		if (scoutAndGoal->goal == Position(0, 0) ||
			scoutAndGoal->reachedGoal) {

			bool gaveNewGoal = false;

			for (auto &spawn : spawns) {
				if (!spawn->hasScout && !spawn->scouted) {
					scoutAndGoal->goal = Position(spawn->location);
					scoutAndGoal->reachedGoal = false;
					spawn->hasScout = true;
					gaveNewGoal = true;
					break;
				}
			}
			
			if (!gaveNewGoal){
				Broodwar->sendText("Returning scout to unithandler");

				scoutUnitsPtr->removeUnit(scoutAndGoal->scout);
				// Send back to base and back to probeUnits for now
				//scoutAndGoal->goal = Position(broodwar->self()->getStartLocation());
				//scoutAndGoal->reachedGoal = false;
			}
		}
		else {
			// Check to see if any better spawns are closer to the
			// scout now
			for (auto &spawn : spawns) {
				if (spawn->hasScout || spawn->scouted) {
					continue;
				}
				
				Position scoutPosition = scoutAndGoal->scout->getPosition();
				if (scoutPosition.getApproxDistance(scoutAndGoal->goal) >
					scoutPosition.getApproxDistance(Position(spawn->location))) {


					for (auto &spawn2 : spawns) {
						if (scoutAndGoal->goal == Position(spawn2->location)) {
							spawn2->hasScout = false;
						}
					}

					spawn->hasScout = true;
					scoutAndGoal->goal = Position(spawn->location);
					hasToUpdateList = true;
				}
				
				
			}
		}
	}

	if (hasToUpdateList) {
		updateSpawnList();
	}

}

void ScoutManager::updateSpawnList() {
	// Do we know for certain where the enemy is now?
	int unScoutedSpawns = 0;
	int unScoutedPos = 0;

	for (u_int i = 0; i < spawns.size(); i++) {
		if (!spawns.at(i)->scouted) {
			unScoutedSpawns++;
			unScoutedPos = i;
		}
	}

	if (unScoutedSpawns == 1) {
		foundEnemyBase(spawns.at(unScoutedPos)->location);
	}

	// Pushes scouted spawns back in the list
	for (u_int i = 0; i < spawns.size() - 1; i++) {
		if (spawns.at(i)->scouted && !spawns.at(i + 1)->scouted) {
			std::swap(spawns.at(i), spawns.at(i + 1));
		}
	}

	// Brings forth the spawns that have an assigned scout
	for (int i = spawns.size() - 1; i > 0; i--) {
		if (spawns.at(i)->scouted) {
			continue;
		}

		if (spawns.at(i)->hasScout && !spawns.at(i - 1)->hasScout) {
			std::swap(spawns.at(i), spawns.at(i - 1));
			Broodwar->sendText("Scout got assigned a better goal");
		}
	}
}

bool ScoutManager::foundEnemyBase(TilePosition position) {
	bool returnBol = foundEnemyBaseInit(position);

	if (returnBol) {
		// Remove any excess scouts
		clearUnusedScouts();
		scoutUnitsPtr->setEnemySpawn(position);
	}

	return returnBol;
}

bool ScoutManager::foundEnemyBaseInit(TilePosition position) {
	// Did we already know where the enemy was
	if (foundEnemySpawn) {
		return false;
	}

	foundEnemySpawn = true;
	peekEnemySpawn = true;
	enemySpawn = position;

	if (strategyDeciderPtr != NULL) {
		strategyDeciderPtr->foundEnemeyBase(position);
	}

	return true;
}

bool ScoutManager::setStrategyDecider(StrategyDecider* strategyPtr) {
	if (strategyDeciderPtr != NULL || strategyPtr == NULL) {
		return false;
	}
	strategyDeciderPtr = strategyPtr;
	return true;
}

bool ScoutManager::beginScouting(ScoutUnits* scoutPtr) {
	if (scoutUnitsPtr != NULL || scoutPtr == NULL){
		return false;
	}
	
	scoutUnitsPtr = scoutPtr;
	isScouting = true;

	// More scoutUnits calls here
	scoutUnitsPtr->assignGoal(Position(spawns.at(0)->location));
	spawns.at(0)->hasScout = true;

	if (foundEnemySpawn) {
		scoutUnitsPtr->setEnemySpawn(enemySpawn);
		strategyDeciderPtr->foundEnemeyBase(enemySpawn);
	}

	return true;
}

bool ScoutManager::clearUnusedScouts() {
	std::vector<ScoutAndGoalStruct*> scouts = scoutUnitsPtr->getScouts();

	// Find the closest unit to the enemy base
	int pos = 0;
	Position enemySpawnPos = Position(enemySpawn);
	for (u_int i = 1; i < scouts.size(); i++) {
		Position scoutPos = scouts.at(i)->scout->getPosition();
		Position curScoutPos = scouts.at(pos)->scout->getPosition();
		if (scoutPos.getApproxDistance(enemySpawnPos) <
			curScoutPos.getApproxDistance(enemySpawnPos)) {

			pos = i;
		}
	}

	// Remove all but the closest unit
	for (u_int i = 0; i < scouts.size(); i++) {
		if (i != pos) {
			scoutUnitsPtr->removeUnit(scouts.at(i)->scout);
		}
	}


	scouts = scoutUnitsPtr->getScouts();

	// We only have one scout now
	ScoutAndGoalStruct* scoutAndGoal = scouts.at(0);
	scoutAndGoal->goal = enemySpawnPos;
	scoutAndGoal->peekEnemyBase = true;

	// Only return true if we have exactly one scout remaining
	return scoutUnitsPtr->getAmountOfScouts() == 1;
}

bool ScoutManager::canAddAnotherScout() {
	return scoutUnitsPtr->getAmountOfScouts() < spawns.size();
}

void ScoutManager::scoutPeekEnemySpawn() {
	if (scoutUnitsPtr->getAmountOfScouts() == 0) {
		return;
	}

	std::vector<ScoutAndGoalStruct*> scouts = scoutUnitsPtr->getScouts();
	ScoutAndGoalStruct* scoutAssignedToPeek;

	// Fetch the scout assigned to peek
	for (auto &scoutAndGoal : scouts) {
		if (scoutAndGoal->peekEnemyBase) {
			scoutAssignedToPeek = scoutAndGoal;
		}
	}

	scoutAssignedToPeek->goal = Position(enemySpawn);
}

// Debug functions
TilePosition::list ScoutManager::getSpawns() {
	TilePosition::list returnLocations;
	for (auto &l : spawns) {
		returnLocations.push_back(l->location);
	}
	return returnLocations;
}

std::vector<bool> ScoutManager::getSpawnBools() {
	std::vector<bool> returnBools;
	for (auto &b : spawns) {
		returnBools.push_back(b->scouted);
	}
	return returnBools;
}

std::vector<bool> ScoutManager::getSpawnHasScouts() {
	std::vector<bool> returnBools;
	for (auto &b : spawns) {
		returnBools.push_back(b->hasScout);
	}
	return returnBools;
}

std::vector<ScoutAndGoalStruct*> ScoutManager::getScouts() {
	return scoutUnitsPtr->getScouts();
}

bool ScoutManager::hasScouts() {
	return scoutUnitsPtr->hasScouts();
}

TilePosition ScoutManager::getEnemySpawn() {
	return enemySpawn;
}


