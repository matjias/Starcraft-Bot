#include "Scouting.h"

using namespace BWAPI;
/*
	TODO: Major expanding of enemy units/structure surveillance
		  and keeping track of enemy movements (if we see it)

	Save units and their positions and maybe when we saw them in
	game ticks since game start and if it is worth saving longer, 
	for example buildings dont move so we permanently (until destroyed)
	save their location but units might be more important to just
	go after the fact that they have it until we kill it.
*/

Scouting::Scouting() { }

Scouting::~Scouting() { }

void Scouting::_init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc, ExampleAIModule* mainProg) {
	TilePosition::list unsortedSpawns = locs;

	// Currently just insertion sorting but should be fine for such a small data set
	for (int i = 0; i < unsortedSpawns.size() - 1; i++) {
		if (unsortedSpawns.at(i) == loc) {
			// We are looking at our spawn position, so we want to move that to
			// the very back of the list
			std::swap(unsortedSpawns.at(i), unsortedSpawns.at(unsortedSpawns.size() - 1));
		}

		for (int j = i; j > 0; j--) {
			// If current spawnLocation is closer than the previous spawn location
			// we move it further to 0, to indicate closest spawn.
			if (unsortedSpawns.at(j).getApproxDistance(loc) < unsortedSpawns.at(j - 1).getApproxDistance(loc)) {
				std::swap(unsortedSpawns.at(j), unsortedSpawns.at(j - 1));
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

	mainProgram = mainProg;
}

bool Scouting::isScouting() {
	// We are scouting as long as we have a scout and as long as we haven't
	// found the enemy's base yet (implicitly have not looped through all spawns)
	return hasAssignedScout();
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
	// ????
	validMove(scout, loc);

	return true;
}

void Scouting::oneScoutAll(Unit u) {
	currentScout = u;

	validMove(currentScout, dynamicLocations.at(0)->location);
}

void Scouting::foundEnemyBase(TilePosition loc) {
	foundEnemy = true;
	enemyBaseLoc = loc;

	// TODO: Clear dynamicLocations list from memory, no longer needed
	//std::vector<LocationStruct*>().swap(dynamicLocations); // Mabbeh wait with memory.. Currently crashes at places.
}

void Scouting::updateScout() {
	if (!hasAssignedScout() || !currentScout->exists() || !currentScout->getPosition().isValid()) {
		return;
	}

	if (isScouting()) {
		if (!foundEnemy) {
			if (hasAssignedScout()) {
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
						validMove(currentScout, dynamicLocations.at(1)->location);
					}
					else {
						validMove(currentScout, dynamicLocations.at(1)->location);
					}

				}

				updateToScoutList();
			}
		}
		else {
			distractEnemyBase();
		}
	}
}

void Scouting::updateToScoutList() {
	// Variable to check if we should move our scout elsewhere
	bool updatedList = false;


	// Go through out location list, except for our own spawn
	for (int i = 0; i < dynamicLocations.size() - 2; i++) {
		if (dynamicLocations.at(i)->scouted && !dynamicLocations.at(i + 1)->scouted) {
			// Move back in list
			std::swap(dynamicLocations.at(i), dynamicLocations.at(i + 1));

			updatedList = true;
		}
		else if (currentScout->getDistance(dynamicLocations.at(i)->location) >
			currentScout->getDistance(dynamicLocations.at(i + 1)->location) &&
			!dynamicLocations.at(i + 1)->scouted) {
			// Move back in list
			std::swap(dynamicLocations.at(i), dynamicLocations.at(i + 1));

			updatedList = true;
		}
	}

	if (updatedList) {
		validMove(currentScout, dynamicLocations.at(0)->location);
	}
}

void Scouting::requestScout() {
	if (foundEnemy && enemyStructures.count(enemyBaseLoc) == 0) {
		// Enemy base has been destroyed, we want an observer
		// not to scout the possible expansion slots for 
		// the remaining foes/structures
		mainProgram->scoutClassRequestedScout(BWAPI::UnitTypes::Protoss_Observer);
	}
}

void Scouting::distractEnemyBase() {
	/* Everything here is testing*/
	Unitset allEnemies = Broodwar->enemy()->getUnits();
	int debugCount = 0;
	for (auto &enemyStuff : allEnemies) {
		if (!enemyStuff->getType().isBuilding()) {
			allEnemies.erase(enemyStuff);
		}
		else {
			Broodwar->drawTextScreen(20, 40 + debugCount * 20, "%s", enemyStuff->getType().c_str());
			
			// Draw enemy we see
			Broodwar->drawEllipseMap(enemyStuff->getPosition(),
				enemyStuff->getType().tileWidth() * TILE_SIZE / 2,
				enemyStuff->getType().tileHeight() * TILE_SIZE / 2,
				Colors::Orange);

			// If they have a weapon we draw the range (cannons etc)
			WeaponType buildWeapon = enemyStuff->getType().groundWeapon();
			if (buildWeapon != NULL) {
				Broodwar->drawCircleMap(enemyStuff->getPosition(),
					buildWeapon.maxRange(),
					Colors::Red);
			}
			
			debugCount++;
		}
	}
	/* End of testing */

	// Make sure we are in the enemy's region,
	// and if we are not, we need to move there
	BWTA::Region *enemyRegion = BWTA::getRegion(returnEnemyBaseLocs());
	BWTA::Region *scoutRegion = BWTA::getRegion(currentScout->getPosition());
	if (scoutRegion != enemyRegion) {
		// Make sure we are moving to the enemy region
		validMove(currentScout, returnEnemyBaseLocs());

		Broodwar->drawTextScreen(20, 40, "Currently moving to enemy spawn");
		// Todo: Ensure we avoid enemy threats that can kill
		//		 us before we get to our target location

	}
	// Otherwise, we must be in their region,
	// and we can start harassing in one form or another
	else {
		// Draw our vision for our scout, and record all enemy units in this range
		Broodwar->drawCircleMap(currentScout->getPosition(), currentScout->getType().sightRange(),
			Colors::Red);

		Unitset enemies = currentScout->getUnitsInRadius(currentScout->getType().sightRange(), 
														Filter::IsEnemy);
		// Draw all, within range, of the enemies maximum attack range
		// Todo: This is what we want to avoid running into if they are attacking
		for (auto &enemyUnit : enemies) {
			Color colorToDraw = enemyUnit->getType().isWorker() ? Colors::Green : Colors::Blue;
			Broodwar->drawCircleMap(enemyUnit->getPosition(), 
				enemyUnit->getType().groundWeapon().maxRange(),
				colorToDraw);
		}





	}


}

void Scouting::recordUnitDiscover(BWAPI::UnitType u, BWAPI::TilePosition loc, int timeTick) {
	BuildingStruct *buildStruct = new BuildingStruct();
	buildStruct->unit = u;
	buildStruct->location = loc;
	buildStruct->scoutedTime = timeTick;
	
	if (u.isBuilding()) {
		// If we do not already know a building
		// at this location we save it for later
		if (enemyStructures.count(loc) == 0) {
			enemyStructures.insert(std::pair<BWAPI::TilePosition, BuildingStruct*>(loc, buildStruct));
		}
	}

}

void Scouting::recordUnitDestroy(BWAPI::UnitType u, BWAPI::TilePosition loc) {
	if (u.isBuilding()) {
		enemyStructures.erase(loc); // Erase should have a check for if the key exists in the map
	}
}


std::map<BWAPI::TilePosition, Scouting::BuildingStruct*, Scouting::CustomMapCompare> Scouting::getEnemyStructures() {
	// Used for our micro and macro to either
	// move to kill the structure, or use for knowledge
	// as to what buildOrder should we continue with
	return enemyStructures;
}

void Scouting::validMove(Unit unitToMove, Position targetLoc) {
	UnitCommand lastCommand(unitToMove->getLastCommand());
	if (lastCommand.getType() != UnitCommandTypes::Move ||
		lastCommand.getTargetPosition() != targetLoc) {

		unitToMove->move(targetLoc);
	}
}

void Scouting::validAttack(Unit unitToAttack, Unit targetUnit) {
	UnitCommand lastCommand(unitToAttack->getLastCommand());
	if (lastCommand.getType() != UnitCommandTypes::Attack_Unit ||
		lastCommand.getTarget() != targetUnit) {

		unitToAttack->attack(targetUnit);
	}
}

void Scouting::enemyBaseDestroyed() {
	// TODO: Logic for scouting the entire map and 
	//		 recorded units/structures for the win
	requestScout(); // A function whose entire purpose is calling one other function...
					// This programming is so brilliant
}

int Scouting::getDistance() {
	if (hasAssignedScout()) {
		return currentScout->getDistance(dynamicLocations.at(0)->location);
	}

	return -1;
}

bool Scouting::endScouting() {
	if (!foundEnemy) {
		return false;
	}

	hasScout = false;
	return true;
}

void Scouting::scoutHasDied() {
	hasScout = false;
}

bool Scouting::isScout(Unit u) {
	return currentScout == u;
}

bool Scouting::returnFoundEnemyBase() {
	return foundEnemy;
}

Position Scouting::returnEnemyBaseLocs() {
	return Position(enemyBaseLoc);
}

void Scouting::set_BWTA_Analyzed() {
	BWTA_isAnalyzed = true;
}

/*
	------!!!!!! DEBUG METHODS BELOW !!!!!!------

	Used for displaying information about scouting to
	the screen, so we can ensure correct behaviour
*/
BWAPI::Position Scouting::getPos() {
	return dynamicLocations.at(0)->location;
}

Position Scouting::getScout() {
	if (hasAssignedScout()) {
		return currentScout->getPosition();
	}

	return Position(-1, -1);
}

TilePosition::list Scouting::getDynamicSpawns() {
	TilePosition::list returnLocs;
	for (auto &u : dynamicLocations) {
		returnLocs.push_back(TilePosition(u->location));
	}

	return returnLocs;
}

std::vector<bool> Scouting::getDynamicSpawnBools() {
	std::vector<bool> returnBools;
	for (auto &u : dynamicLocations) {
		returnBools.push_back(u->scouted);
	}

	return returnBools;
}
