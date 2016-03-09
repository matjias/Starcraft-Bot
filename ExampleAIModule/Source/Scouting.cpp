#include "Scouting.h"

using namespace BWAPI;


struct ScoutStruct {
	Unit scout;
	Position location;
	bool scouted;
};

struct LocationStruct {
	Position location;
	bool scouted;
};

std::vector<ScoutStruct*> currentScouts;
//TilePosition::list spawnLocations;

std::vector<LocationStruct*> currentLocations;

bool foundEnemy;
TilePosition enemyBaseLoc;

Scouting::Scouting() { }

Scouting::~Scouting() { }

void Scouting::_init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc) {
	TilePosition::list unsortedSpawns = locs;

	// TODO: Check logic...  (logic checked and should now be less flawed)
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
		}
	}

	for (int i = 0; i < unsortedSpawns.size(); i++) {
		LocationStruct *locStruct = new LocationStruct();
		locStruct->location = Position(unsortedSpawns.at(i));
		locStruct->scouted = false;
		currentLocations.push_back(locStruct);
	}

	//spawnLocations = unsortedSpawns;
}

bool Scouting::isScouting() {
	if (currentScouts.size() > 0) 
		return true;

	return false;
}

bool Scouting::assignScout(Unit scout) {
	if (foundEnemy || currentScouts.size() >= currentLocations.size() - 1) {
		// Currently not going to assign a scout if we have found the enemy
		// or if we have enough scouts out to check every location at once
		return false;
	}

	// Scouts everything in the sorted list
	oneScoutAll(scout);
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
	ScoutStruct *toPush = new ScoutStruct();
	toPush->scout = scout;
	toPush->location = loc;

	currentScouts.push_back(toPush);
	scout->move(loc);

	return true;
}

void Scouting::oneScoutAll(Unit u) {
	for (int i = 0; i < currentLocations.size() - 1; i++) {
		if (i == 0) {
			u->move(currentLocations.at(i)->location);
		}
		else {
			u->move(currentLocations.at(i)->location, true);
		}

		ScoutStruct *toPush = new ScoutStruct();
		toPush->scout = u;
		toPush->location = currentLocations.at(i)->location;

		currentScouts.push_back(toPush);
	}
}

void Scouting::foundEnemyBase(TilePosition loc) {
	foundEnemy = true;
	enemyBaseLoc = loc;
}

void Scouting::updateScout() {
	/*if (currentScouts.size() > 0) {
		currentScouts.at(0)->scout->getDistance(Position(currentScouts.at(0)->location))
	}*/
}

int Scouting::getDistance() {
	// Currently assumes only one scout
	if (currentScouts.size() > 0) {
		return currentScouts.at(0)->scout->getDistance(currentScouts.at(0)->location);
	}
	else {
		return -1;
	}
}

bool Scouting::endScouting() {
	if (currentScouts.size() > 0) {
		return false;
	}

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
	if (currentScouts.size() > 0) {
		return currentScouts.at(0)->location.x;
	}
	return -1;
}

int Scouting::getY() {
	if (currentScouts.size() > 0) {
		return currentScouts.at(0)->location.y;
	}
	return -1;
}

TilePosition::list Scouting::getSpawns() {
	TilePosition::list returnLocs;
	for (int i = 0; i < currentLocations.size(); i++) {
		returnLocs.push_back(TilePosition(currentLocations.at(i)->location));
	}
	
	return returnLocs;
}