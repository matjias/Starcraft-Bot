#pragma once
#include "ScoutManager.h"

// Most of our data types are either ScoutManager or BWAPI
using namespace BWAPI;

ScoutManager::ScoutManager() {
	/* 
		ScoutManager on the constructor creates a list of
		potential spawn locations our enemy could be in
		and sorts them based on their distance to our own spawn
	*/

	TilePosition::list unsortedSpawns;
	TilePosition ourSpawn = Broodwar->self()->getStartLocation();

	// Adds all the spawns to a list, excluding our own spawn
	for (auto &location : Broodwar->getStartLocations()) {
		if (location != Broodwar->self()->getStartLocation()) {
			unsortedSpawns.push_back(location);
		}
	}

	// Presorts the start locations so the shortest spawn
	// to our spawn is first in the list, and our spawn is
	// Uses insertion sort, but the size of spawn locations
	// is small enough for this to not be an issue
	for (unsigned int i = 1; i < unsortedSpawns.size(); i++) {
		for (int j = i; j > 0; j--) {
			if (unsortedSpawns.at(j).getApproxDistance(ourSpawn) <
				unsortedSpawns.at(j - 1).getApproxDistance(ourSpawn)) {

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
}

ScoutManager::~ScoutManager() { }

void ScoutManager::recordUnitDiscover(Unit u) {
	// Do we already know about this unit?
	if (enemyUnits.count(u->getID()) == 1) {
		// Then we update their position and when we last saw it
		enemyUnits.at(u->getID())->lastKnownPosition = u->getPosition();
		enemyUnits.at(u->getID())->lastScouted = Broodwar->getFrameCount();
	}
	// Otherwise we need to record it
	else {
		UnitStruct *uStruct = new UnitStruct();
		uStruct->unit = u;
		uStruct->lastKnownPosition = u->getPosition();
		uStruct->lastScouted = Broodwar->getFrameCount();

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
		Broodwar->sendText("A unit was destroyed which was not recorded");
	}

	return elementsRemoved;
}


void ScoutManager::updateScoutManager() {

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
