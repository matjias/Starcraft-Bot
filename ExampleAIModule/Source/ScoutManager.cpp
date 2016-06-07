#pragma once
#include "ScoutManager.h"

// Most of our data types are either ScoutManager or BWAPI
using namespace BWAPI;

ScoutManager::ScoutManager() { }

ScoutManager::ScoutManager(Game *gamePtr) {
	broodwar = gamePtr;
}

ScoutManager::~ScoutManager() { }

bool ScoutManager::_init() {
	// If we have already defined called _init once 
	// and defined the spawns, then we just stop
	if (spawns.size() > 0) {
		return false;
	}

	TilePosition::list unsortedSpawns;
	TilePosition ownSpawn = broodwar->self()->getStartLocation();

	// Adds all the spawns to a list, excluding our own spawn
	for (auto &location : broodwar->getStartLocations()) {
		if (location != ownSpawn) {
			unsortedSpawns.push_back(location);
		}
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

	// Edge case for 1v1 map, because we'd know where the enemy is

	return true;
}

void ScoutManager::recordUnitDiscover(Unit u) {
	// Do we already know about this unit?
	if (enemyUnits.count(u->getID()) == 1) {
		// Then we update their position and when we last saw it
		enemyUnits.at(u->getID())->lastKnownPosition = u->getPosition();
		enemyUnits.at(u->getID())->lastScouted = broodwar->getFrameCount();
	}
	// Otherwise we need to record it
	else {
		UnitStruct *uStruct = new UnitStruct();
		uStruct->unit = u;
		uStruct->lastKnownPosition = u->getPosition();
		uStruct->lastScouted = broodwar->getFrameCount();

		enemyUnits.insert(std::pair<int, UnitStruct*>(u->getID(), uStruct));
	}
}

int ScoutManager::recordUnitDestroy(Unit u) {
	int elementsRemoved = enemyUnits.erase(u->getID());

	// Something went wrong if we destroyed a unit
	// we did not have a record of
	if (elementsRemoved == 0) {
		// TODO: Currently just tells the player about it,
		// but this should be covered in a unit test
		broodwar->sendText("A unit was destroyed which was not recorded");
	}

	return elementsRemoved;
}

void ScoutManager::recordUnitEvade(Unit u) {
	if (enemyUnits.count(u->getID()) == 1) {
		enemyUnits.at(u->getID())->lastKnownPosition = u->getPosition();
		enemyUnits.at(u->getID())->lastScouted = broodwar->getFrameCount();
	}
	else {
		// Something went wrong, for now just tell the client
		broodwar->sendText("A unit has evaded that wasnt recorded");
	}
}


void ScoutManager::updateScoutManager() {
	if (!isScouting) {
		return;
	}
	
	if (!foundEnemySpawn) {
		findEnemySpawn();
	}

	scoutUnits->updateScouts();
}

void ScoutManager::findEnemySpawn() {
	std::vector<ScoutAndGoalStruct*> scouts = scoutUnits->getScouts();
	bool hasToUpdateList = false;

	// Now we update if any of our scouts see their target
	for (auto &scoutAndGoal : scouts) {

		if (scoutAndGoal->goal == Position(0, 0)) {
			continue;
		}


		int sightRadius = scoutAndGoal->scout->getType().sightRange();
		TilePosition goalPosition = TilePosition(scoutAndGoal->goal);
		if (broodwar->isVisible(goalPosition)) {

			for (auto &spawn : spawns) {
				if (goalPosition == spawn->location) {
					spawn->scouted = true;
					scoutAndGoal->reachedGoal = true;
					hasToUpdateList = true;
				}

			}
		}
	}

	// Check to make sure all scouts have a locaton to go to
	for (auto &scoutAndGoal : scouts) {
		// Do we have a scout that has not been assigned a goal yet?
		if (scoutAndGoal->goal == Position(0, 0) ||
			scoutAndGoal->reachedGoal) {
			// Find next spawn that has not been scouted 
			// yet, and that does not have a scout
			for (auto &spawn : spawns) {
				if (!spawn->hasScout && !spawn->scouted) {
					scoutAndGoal->goal = Position(spawn->location);
					spawn->hasScout = true;
					break;
				}
			}
			
			if (scoutAndGoal->goal == Position(0, 0)) {
				broodwar->sendText("Scout could not get new scouting location");
			}
		}
		else {
			// Check to see if any better spawns are closer to the
			// scout now
			for (auto &spawn : spawns) {
				if (!spawn->hasScout && !spawn->scouted) {
					Position scoutPosition = scoutAndGoal->scout->getPosition();
					if (scoutPosition.getApproxDistance(scoutAndGoal->goal) >
						scoutPosition.getApproxDistance(Position(spawn->location))) {

						for (auto &spawn2 : spawns) {
							if (Position(spawn2->location) = scoutAndGoal->goal) {
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
	}

	//if (hasToUpdateList) {
		updateSpawnList();
	//}

	for (auto &scoutAndGoal : scouts) {
		//scoutAndGoal->scout->move(scoutAndGoal->goal);
	}

}

void ScoutManager::updateSpawnList() {
	// Do we know for certain where the enemy is now?
	if (spawns.size() > 1 && spawns.at(1)->scouted) {
		foundEnemyBase(spawns.at(1)->location);
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
			broodwar->sendText("Swapped hasScout shit");
		}
	}
}

bool ScoutManager::foundEnemyBase(TilePosition position) {
	// Did we already know where the enemy was
	if (foundEnemySpawn) {
		return false;
	}

	foundEnemySpawn = true;
	enemySpawn = position;
	
	return true;
}

bool ScoutManager::beginScouting(ScoutUnits* scoutUnitsPtr) {
	if (scoutUnits != NULL || scoutUnitsPtr == NULL) {
		return false;
	}
	
	scoutUnits = scoutUnitsPtr;
	isScouting = true;

	// More scoutUnits calls here
	scoutUnits->assignGoal(Position(spawns.at(0)->location));
	spawns.at(0)->hasScout = true;

	return true;
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
	return scoutUnits->getScouts();
}

bool ScoutManager::hasScouts() {
	return scoutUnits->hasScouts();
}