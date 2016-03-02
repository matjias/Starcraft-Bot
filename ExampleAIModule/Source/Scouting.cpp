#include "Scouting.h"

using namespace BWAPI;


struct ScoutStruct {
	Unit scout;
	TilePosition location;
};

std::vector<ScoutStruct*> currentScouts;
TilePosition::list spawnLocations;
TilePosition ownLocation;

void init(TilePosition::list locs, TilePosition loc) {
	spawnLocations = locs;
	ownLocation = loc;
}

bool isScouting() {
	if (currentScouts.size() > 0) 
		return true;

	return false;
}

bool assignScout(Unit scout) {
	if (currentScouts.size() >= spawnLocations.size() - 1) {
		return false;
	}

	// Get location to scout
	TilePosition locationToScout;

	for (int i = 0; i < spawnLocations.size(); i++) {
		if (spawnLocations.at(i) != ownLocation) {
			// See if that location is already being scouted
			for (int j = 0; j < currentScouts.size(); j++) {
				if (currentScouts.at(j)->location != spawnLocations.at(i)) {
					locationToScout = spawnLocations.at(i);
				}
			}

		}
	}

	ScoutStruct *toPush = new ScoutStruct();
	toPush->location = locationToScout;
	toPush->scout = scout;


	currentScouts.push_back(toPush);
}

void updateScout() {

}

bool endScouting() {
	if (currentScouts.size() > 0) {
		return false;
	}

	// Clear memory here
}