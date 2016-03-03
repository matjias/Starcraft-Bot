#include "Scouting.h"

using namespace BWAPI;


struct ScoutStruct {
	Unit scout;
	Position location;
};

std::vector<ScoutStruct*> currentScouts;
TilePosition::list spawnLocations;
bool foundEnemy;
TilePosition enemyBaseLoc;

Scouting::Scouting() { }

Scouting::~Scouting() { }

void Scouting::_init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc) {
	TilePosition::list unsortedSpawns = locs;

	// TODO: Check logic... 
	// Currently just insertion sorting but should be fine for such a small data set
	for (int i = 0; i < unsortedSpawns.size(); i++) {
		for (int j = i; j > 1; j--) {
			if (unsortedSpawns.at(i) != loc) {
				// If current spawnLocation is closer than the previous spawn location
				// we move it further to 0, to indicate closest spawn.
				if (unsortedSpawns.at(j).getDistance(loc) < unsortedSpawns.at(j - 1).getDistance(loc)) {
					TilePosition buf = unsortedSpawns.at(j - 1);
					unsortedSpawns.at(j - 1) = unsortedSpawns.at(j);
					unsortedSpawns.at(j) = buf;
				}
			} else { 
				// We want our spawn at the end so we can ignore it in future code
				TilePosition buf = unsortedSpawns.at(unsortedSpawns.size() - 1);
				unsortedSpawns.at(unsortedSpawns.size() - 1) = unsortedSpawns.at(i);
				unsortedSpawns.at(i) = buf;
			}
		}
	}

	spawnLocations = unsortedSpawns;
}

bool Scouting::isScouting() {
	if (currentScouts.size() > 0) 
		return true;

	return false;
}

bool Scouting::assignScout(Unit scout) {
	if (currentScouts.size() >= spawnLocations.size() - 1) {
		return false;
	}

	// Get location to scout
	Position locationToScout;

	for (int i = 0; i < spawnLocations.size() - 1; i++) {
		// Check if we already have scouts out
		// TODO: Fix logic, list should be ordered now for easier scouting
		if (currentScouts.size() > 0) {
			for (int j = 0; j < currentScouts.size(); j++) {
				if (TilePosition(currentScouts.at(j)->location) != spawnLocations.at(i)) {
					locationToScout = Position(spawnLocations.at(i));
				}
			}
		} else {
			locationToScout = Position(spawnLocations.at(i));
		}
	}

	ScoutStruct *toPush = new ScoutStruct();
	toPush->location = locationToScout;
	toPush->scout = scout;

	currentScouts.push_back(toPush);

	// Begin scouting
	scout->move(locationToScout);

	return true;
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
	return spawnLocations;
}