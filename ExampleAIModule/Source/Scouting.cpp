#include "Scouting.h"

using namespace BWAPI;


struct ScoutStruct {
	Unit scout;
	Position location;
};

std::vector<ScoutStruct*> currentScouts;
TilePosition::list spawnLocations;
TilePosition ownLocation;

Scouting::Scouting() { }

Scouting::~Scouting() { }

void Scouting::_init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc) {
	spawnLocations = locs;
	ownLocation = loc;
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

	for (int i = 0; i < spawnLocations.size(); i++) {
		if (spawnLocations.at(i) != ownLocation) {
			// See if that location is already being scouted

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
	}

	ScoutStruct *toPush = new ScoutStruct();
	toPush->location = locationToScout;
	toPush->scout = scout;

	currentScouts.push_back(toPush);

	// Begin scouting
	scout->move(locationToScout);

	return true;
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