#include "Scouting.h"

using namespace BWAPI;

struct LocationStruct {
	Position location;
	bool scouted;
};

Unit currentScout;
bool hasScout;
std::vector<LocationStruct*> dynamicLocations;

bool foundEnemy, knowsEnemy;
TilePosition enemyBaseLoc;

Scouting::Scouting() { }

Scouting::~Scouting() { }

void Scouting::_init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc) {
	TilePosition::list unsortedSpawns = locs;

	// Currently just insertion sorting but should be fine for such a small data set
	for (int i = 0; i < unsortedSpawns.size() - 1; i++) {
		if (unsortedSpawns.at(i) == loc) {
			// We are looking at our spawn position, so we want to move that to
			// the very back of the list
			TilePosition buf = unsortedSpawns.at(unsortedSpawns.size() - 1);
			unsortedSpawns.at(unsortedSpawns.size() - 1) = unsortedSpawns.at(i);
			unsortedSpawns.at(i) = buf;
		}

		for (int j = i; j > 0; j--) {
			// If current spawnLocation is closer than the previous spawn location
			// we move it further to 0, to indicate closest spawn.
			if (unsortedSpawns.at(j).getApproxDistance(loc) < unsortedSpawns.at(j - 1).getApproxDistance(loc)) {
				TilePosition buf = unsortedSpawns.at(j - 1);
				unsortedSpawns.at(j - 1) = unsortedSpawns.at(j);
				unsortedSpawns.at(j) = buf;
			}
		}
	}

	for (int i = 0; i < unsortedSpawns.size(); i++) {
		LocationStruct *locStruct = new LocationStruct();
		locStruct->location = Position(unsortedSpawns.at(i));

		if (i == unsortedSpawns.size() - 1) {
			locStruct->scouted = true; // This is our own spawn, so we have already 'scouted' it
		} else {
			locStruct->scouted = false;
		}
		
		dynamicLocations.push_back(locStruct);
	}
}

bool Scouting::isScouting() {
	if (hasAssignedScout() && (!foundEnemy && !dynamicLocations.at(0)->scouted))
		return true;

	return false;
}

bool Scouting::hasAssignedScout() {
	return hasScout;
}

bool Scouting::assignScout(Unit scout) {
	if (foundEnemy) {
		// Currently not going to assign a scout if we have found the enemy
		// or if we have enough scouts out to check every location at once
		return false;
	}
	oneScoutAll(scout);
	hasScout = true;
	return true;
}

bool Scouting::assignScoutandLoc(Unit scout, Position loc) {
	scout->move(loc);

	return true;
}

void Scouting::oneScoutAll(Unit u) {
	currentScout = u;

	currentScout->move(dynamicLocations.at(0)->location);
}

void Scouting::foundEnemyBase(TilePosition loc) {
	foundEnemy = true;
	enemyBaseLoc = loc;
}

void Scouting::updateScout() {
	// One scout tactic

	// Edge case - 1v1 map
	if (dynamicLocations.size() == 2) {
		foundEnemyBase(TilePosition(dynamicLocations.at(0)->location));
	}

	if (currentScout->getType().sightRange() * currentScout->getType().sightRange() <=
		dynamicLocations.at(0)->location.x * dynamicLocations.at(0)->location.x +
		dynamicLocations.at(0)->location.y * dynamicLocations.at(0)->location.y &&
		Broodwar->isVisible(TilePosition(dynamicLocations.at(0)->location))) {
		dynamicLocations.at(0)->scouted = true;

		if (dynamicLocations.size() > 2 && dynamicLocations.at(2)->scouted) {
			foundEnemyBase(TilePosition(dynamicLocations.at(2)->location));
			currentScout->move(dynamicLocations.at(1)->location);
		}
		else {
			currentScout->move(dynamicLocations.at(1)->location);
		}

	}

	updateToScoutList();
}

void Scouting::updateToScoutList() {
	// Variable to check if we should move our scout elsewhere
	bool updatedList = false;

	// No need for all these if we already know where the enemy is
	if (!foundEnemy) {

		// Go through out location list, except for our own spawn
		for (int i = 0; i < dynamicLocations.size() - 2; i++) {
			if (dynamicLocations.at(i)->scouted && !dynamicLocations.at(i + 1)->scouted) {
				// Move back in list
						LocationStruct *tempLoc = new LocationStruct();
						tempLoc = dynamicLocations.at(i);
						dynamicLocations.at(i) = dynamicLocations.at(i + 1);
						dynamicLocations.at(i + 1) = tempLoc;

						updatedList = true;
			}
			else if (currentScout->getDistance(dynamicLocations.at(i)->location) >
				currentScout->getDistance(dynamicLocations.at(i + 1)->location) &&
				!dynamicLocations.at(i + 1)->scouted) {
				// Move back in list
				LocationStruct *tempLoc = new LocationStruct();
				tempLoc = dynamicLocations.at(i);
				dynamicLocations.at(i) = dynamicLocations.at(i + 1);
				dynamicLocations.at(i + 1) = tempLoc;

				updatedList = true;
			}
		}
	}

	if (updatedList) {
		currentScout->move(dynamicLocations.at(0)->location);
	}
}

int Scouting::getDistance() {
	if (hasAssignedScout()) {
		return currentScout->getDistance(dynamicLocations.at(0)->location);
	}

	return -1;
}

bool Scouting::endScouting() {
	return false;

	// Clear memory here
}


// DEBUG METHODS
bool Scouting::returnFoundEnemyBase() {
	return foundEnemy;
}

Position Scouting::returnEnemyBaseLocs() {
	return Position(enemyBaseLoc);
}

int Scouting::getX() {
	return dynamicLocations.at(0)->location.x;

	return -1;
}

int Scouting::getY() {
	return dynamicLocations.at(0)->location.y;

	return -1;
}

Position Scouting::getScout() {
	if (hasAssignedScout()) {
		return currentScout->getPosition();
	}

	return Position(-1, -1);
}

TilePosition::list Scouting::getSpawns() {
	TilePosition::list returnLocs;
	for (int i = 0; i < dynamicLocations.size(); i++) {
		returnLocs.push_back(TilePosition(dynamicLocations.at(i)->location));
	}
	
	return returnLocs;
}

bool Scouting::isScout(Unit u) {
	if (currentScout == u) {
		return true;
	}

	return false;
}

TilePosition::list Scouting::getDynamicSpawns() {
	TilePosition::list returnLocs;
	for (int i = 0; i < dynamicLocations.size(); i++) {
		returnLocs.push_back(TilePosition(dynamicLocations.at(i)->location));
	}

	return returnLocs;
}

std::vector<bool> Scouting::getDynamicSpawnBools() {
	std::vector<bool> returnBools;
	for (int i = 0; i < dynamicLocations.size(); i++) {
		returnBools.push_back(dynamicLocations.at(i)->scouted);
	}

	return returnBools;
}
