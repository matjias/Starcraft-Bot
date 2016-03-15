#include "Scouting.h"

using namespace BWAPI;


/* Eleminating Multiple Scouts
struct ScoutStruct {
	Unit scout;
	Position location;
	bool scouted;
};
*/

struct LocationStruct {
	Position location;
	bool scouted;
};

Unit currentScout;
bool hasScout;
//std::vector<ScoutStruct*> currentScouts;
//TilePosition::list spawnLocations;

//std::vector<LocationStruct*> currentLocations;
std::vector<LocationStruct*> dynamicLocations;

bool foundEnemy, knowsEnemy;
TilePosition enemyBaseLoc;

Scouting::Scouting() { }

Scouting::~Scouting() { }

void Scouting::_init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc) {
	TilePosition::list unsortedSpawns = locs;

	// Currently just insertion sorting but should be fine for such a small data set
	for (int i = 0; i < unsortedSpawns.size() - 1; i++) {
		if (unsortedSpawns.at(i) != loc) {
			for (int j = i; j > 0; j--) {
				// If current spawnLocation is closer than the previous spawn location
				// we move it further to 0, to indicate closest spawn.
				if (unsortedSpawns.at(j).getApproxDistance(loc) < unsortedSpawns.at(j - 1).getApproxDistance(loc)) {
					TilePosition buf = unsortedSpawns.at(j - 1);
					unsortedSpawns.at(j - 1) = unsortedSpawns.at(j);
					unsortedSpawns.at(j) = buf;
				}
			}
		} else {
			// We want our spawn at the end so we can ignore it in future code
			// which is also why we're only going to size - 1, because the last spot is reserved for that
			TilePosition buf = unsortedSpawns.at(unsortedSpawns.size() - 1);
			unsortedSpawns.at(unsortedSpawns.size() - 1) = unsortedSpawns.at(i);
			unsortedSpawns.at(i) = buf;

			// Still need to sort what we just changed, because we cannot
			// ensure that it is unsorted from the switch
			for (int j = i; j > 0; j--) {
				if (unsortedSpawns.at(j).getApproxDistance(loc) < unsortedSpawns.at(j - 1).getApproxDistance(loc)) {
					TilePosition buf = unsortedSpawns.at(j - 1);
					unsortedSpawns.at(j - 1) = unsortedSpawns.at(j);
					unsortedSpawns.at(j) = buf;
				}
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

	//spawnLocations = unsortedSpawns;
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
	if (foundEnemy /*|| currentScouts.size() >= currentLocations.size() - 1*/) {
		// Currently not going to assign a scout if we have found the enemy
		// or if we have enough scouts out to check every location at once
		return false;
	}

	// Scouts everything in the sorted list
	oneScoutAll(scout);
	hasScout = true;
	return true;



	// Get location to scout
	//Position locationToScout;

	
	//for (int i = 0; i < currentLocations.size() - 1; i++) {
		// Check if we already have scouts out
		// TODO: Fix logic, list should be ordered now for easier scouting
		/*if (currentScouts.size() > 0) {
			for (int j = 0; j < currentScouts.size(); j++) {
				if (TilePosition(currentScouts.at(j)->location) != spawnLocations.at(i)) {
					locationToScout = Position(spawnLocations.at(i));
					break;
				}
			}
		} else {*/
	//	locationToScout = Position(currentLocations.at(i)->location);
	//		break;
		//}
	//}

	//return assignScoutandLoc(scout, locationToScout);
}

bool Scouting::assignScoutandLoc(Unit scout, Position loc) {
	/*ScoutStruct *toPush = new ScoutStruct();
	toPush->scout = scout;
	toPush->location = loc;

	currentScouts.push_back(toPush);*/
	scout->move(loc);

	return true;
}

void Scouting::oneScoutAll(Unit u) {
	currentScout = u;

	//for (int i = 0; i < dynamicLocations.size() - 1; i++) {
		//if (i == 0) {
	currentScout->move(dynamicLocations.at(0)->location);
		//}
		//else {
		//	u->move(currentLocations.at(i)->location, true);
		//}

		
		//ScoutStruct *toPush = new ScoutStruct();
		//toPush->scout = u;
		//toPush->location = currentLocations.at(i)->location;

		//currentScouts.push_back(toPush);
	//}
}

void Scouting::foundEnemyBase(TilePosition loc) {
	foundEnemy = true;
	enemyBaseLoc = loc;
}

void Scouting::updateScout() {
	/*if (currentScouts.size() > 0) {
		currentScouts.at(0)->scout->getDistance(Position(currentScouts.at(0)->location))
	}*/

	// One scout tactic

	// Edge case - 1v1 map
	//if (currentScout->exists()) {
		if (dynamicLocations.size() == 2) {
			foundEnemyBase(TilePosition(dynamicLocations.at(0)->location));
		}

		// Finds the next scout in the list
		//ScoutStruct *validScout;
		//int i = 0; // We want to save this i for later
		//while (i < currentScouts.size()) {
		//	if (currentScouts.at(i)->location == tempUpdatedLocations.at(0)->location) {
		//		validScout = currentScouts.at(i);
		//		break;
		//	}
		//	i++;
		//}

		if (currentScout->getType().sightRange() * currentScout->getType().sightRange() <=
			currentScout->getPosition().x * currentScout->getPosition().x +
			currentScout->getPosition().y * currentScout->getPosition().y &&
			Broodwar->isVisible(TilePosition(currentScout->getPosition()))) {
			dynamicLocations.at(0)->scouted = true;

			if (dynamicLocations.size() > 2 && dynamicLocations.at(1)->scouted) {
				foundEnemyBase(TilePosition(dynamicLocations.at(1)->location));
			}
			else {
				currentScout->move(dynamicLocations.at(1)->location);
			}

		}

		updateToScoutList();
	//}
	
}

void Scouting::updateToScoutList() {
	/* 
		!!!!! CURRENTLY CRASHES THE GAME !!!!!
		When the tempUpdatedLocations is swapping places it crashes for some
		weird reason, have yet to identify what it is (did not have this problem earlier
		so source of the problem might not even be in this function).
	*/


	// Variable to check if we should move our scout elsewhere
	bool updatedList = false;

	// No need for all these if we already know where the enemy is
	if (!foundEnemy) {
		

		// Go through out location list, except for our own spawn
		for (int i = 0; i < dynamicLocations.size() - 2; i++) {
			if (dynamicLocations.at(i)->scouted) {
				// Move back in list
				for (int j = i; j < dynamicLocations.size() - 1; j++) {
					if (!dynamicLocations.at(j + 1)->scouted) {
						LocationStruct *tempLoc = new LocationStruct;
						tempLoc = dynamicLocations.at(j);
						dynamicLocations.at(j) = dynamicLocations.at(j + 1);
						dynamicLocations.at(j + 1) = tempLoc;

						updatedList = true;
					} else {
						// Finished looping through our list everything, we can break now
						j = dynamicLocations.size();
					}
				}
			}
			else if (currentScout->getDistance(dynamicLocations.at(i)->location) >
				currentScout->getDistance(dynamicLocations.at(i + 1)->location)) {
				// Move back in list
				for (int j = i; j < dynamicLocations.size() - 1; j++) {
					if (currentScout->getDistance(dynamicLocations.at(j)->location) >
						currentScout->getDistance(dynamicLocations.at(j + 1)->location)) {
						LocationStruct *tempLoc = new LocationStruct;
						tempLoc = dynamicLocations.at(j);
						dynamicLocations.at(j) = dynamicLocations.at(j + 1);
						dynamicLocations.at(j + 1) = tempLoc;

						updatedList = true;
					}
				}
			}
		}
	}

	if (updatedList) {
		//for (int i = 0; i < currentScouts.size(); i++) {
		//	if (!currentScouts.at(i)->scouted) {
		//		currentScouts.at(i)->scout->move(tempUpdatedLocations.at(0)->location);
		//	}
		//}
		currentScout->move(dynamicLocations.at(0)->location);
	}
}

int Scouting::getDistance() {
	// Currently assumes only one scout
	/*if (currentScouts.size() > 0) {
		return currentScouts.at(0)->scout->getDistance(currentScouts.at(0)->location);
	}
	else {
		return -1;
	}*/
	/*
	for (int i = 0; i < currentScouts.size(); i++) {
		if (!currentScouts.at(i)->scouted) {
			return currentScouts.at(i)->scout->getDistance(currentScouts.at(i)->location);
		}
	}
	*/
	if (hasAssignedScout()) {
		return currentScout->getDistance(dynamicLocations.at(0)->location);
	}

	return -1;
}

bool Scouting::endScouting() {
	//if (currentScouts.size() > 0) {
		return false;
	//}

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
	/*for (int i = 0; i < currentScouts.size(); i++) {
		if (!currentScouts.at(i)->scouted) {
			return currentScouts.at(i)->location.x;
		}
	}*/

	return dynamicLocations.at(0)->location.x;

	return -1;
}

int Scouting::getY() {
	/*for (int i = 0; i < currentScouts.size(); i++) {
		if (!currentScouts.at(i)->scouted) {
			return currentScouts.at(i)->location.y;
		}
	}*/

	return dynamicLocations.at(0)->location.y;

	return -1;
}

Position Scouting::getScout() {
	/*for (int i = 0; i < currentScouts.size(); i++) {
		if (!currentScouts.at(i)->scouted) {
			return currentScouts.at(i)->scout->getPosition();
		}
	}*/

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
	/*for (int i = 0; i < currentScouts.size(); i++) {
		if (u == currentScouts[i]->scout) {
			return true;
		}
	}*/

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
