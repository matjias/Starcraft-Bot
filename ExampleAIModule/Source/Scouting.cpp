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
	//std::vector<LocationStruct*>().swap(dynamicLocations); 
	// Mabbeh wait with memory.. Currently crashes at places.

	// We found their base, now calculate their expansion places
	findEnemyExpansions();
}

void Scouting::findEnemyExpansions() {
	enemyRegion = BWTA::getRegion(returnEnemyBaseLocs());
	if (enemyRegion != NULL) {
		enemyBaseLocation = BWTA::getNearestBaseLocation(returnEnemyBaseLocs());
		if (enemyBaseLocation != NULL) {
			const double FACTOR = 2;

			std::set<BWTA::BaseLocation*> allBaseLocations = BWTA::getBaseLocations();

			BWTA::BaseLocation* currentClosestExpansion;
			std::vector<BWTA::BaseLocation*> currentValidExpansions;

			double shortestDist = std::numeric_limits<double>::max();

			for (auto &baseLocation : allBaseLocations) {
				// Continue if we do not care about this expansion place
				if (baseLocation == enemyBaseLocation || baseLocation->isIsland() ||
					baseLocation == BWTA::getStartLocation(Broodwar->self())) {

					continue;
				}

				// Is this the first time we're looping through?
				if (currentClosestExpansion == NULL) {
					currentClosestExpansion = baseLocation;
					currentValidExpansions.push_back(baseLocation);
					shortestDist = enemyBaseLocation->getGroundDistance(baseLocation);
				}

				// Else we check if the baseLocation is even worth considering
				else if (enemyBaseLocation->getGroundDistance(baseLocation) <= shortestDist * FACTOR) {
					// Did we find a new shortest expansion?
					if (enemyBaseLocation->getGroundDistance(baseLocation) < shortestDist) {
						currentClosestExpansion = baseLocation;
						shortestDist = enemyBaseLocation->getGroundDistance(baseLocation);

						// Update our validEnemyExpansions vector
						std::vector<BWTA::BaseLocation*> temp;
						temp.push_back(baseLocation);
						for (int i = 0; i < currentValidExpansions.size(); i++) {
							if (enemyBaseLocation->getGroundDistance(currentValidExpansions.at(i)) <= shortestDist * FACTOR) {
								temp.push_back(currentValidExpansions.at(i));
							}
						}

						currentValidExpansions = temp;
					}
					// If we didn't we just add it to our vector of valid expansions
					else {
						currentValidExpansions.push_back(baseLocation);
					}
				}
			}
			currentValidExpansions.shrink_to_fit();

			enemyExpansion = currentClosestExpansion;
			enemyExpansions = currentValidExpansions;
		}
	}
}

BWTA::BaseLocation* Scouting::closestEnemyExpansion() {
	return enemyExpansion;
}

std::vector<BWTA::BaseLocation*> Scouting::closestEnemyExpansions() {
	if (enemyExpansions.size() > 0) {
		return enemyExpansions;
	}
	else {
		return std::vector<BWTA::BaseLocation*>(); // Returns an empty vector instead of null
	}
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
						foundEnemyBase(TilePosition(dynamicLocations.at(1)->location));
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
			// Check if we havent yet recorded enemyRegion and ensure we do so
			if (enemyRegion == NULL || enemyBaseLocation == NULL) {
				findEnemyExpansions();
			}

			// Start harassing enemy base
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
	// Make sure we are in the enemy's region,
	// and if we are not, we need to move there
	BWTA::Region *enemyRegion = BWTA::getRegion(returnEnemyBaseLocs());
	BWTA::Region *scoutRegion = BWTA::getRegion(currentScout->getPosition());
	if (scoutRegion != enemyRegion) {
		// Make sure we are moving to the enemy region
		validMove(currentScout, returnEnemyBaseLocs());

		// Todo: Ensure we avoid enemy threats that can kill
		//		 us before we get to our target location

	}
	// Otherwise, we must be in their region,
	// and we can start harassing in one form or another
	else {
		// Visualizing methods for enemy units and our scout

		// Draw our vision for our scout, and record all enemy units in this range
		Broodwar->drawCircleMap(currentScout->getPosition(), currentScout->getType().sightRange(),
			Colors::Cyan);
		// Draw our attack range
		Broodwar->drawCircleMap(currentScout->getPosition(), 
			currentScout->getType().groundWeapon().maxRange(),
			Colors::Cyan);

		Unitset enemies = currentScout->getUnitsInRadius(currentScout->getType().sightRange(), 
														Filter::IsEnemy && Filter::CanAttack);
		std::vector<CustomVector> vectors;
		// Draw all, within range, of the enemies maximum attack range
		// Todo: This is what we want to avoid running into if they are attacking
		for (auto &enemyUnit : enemies) {
			Color colorToDraw = enemyUnit->getType().isWorker() ? Colors::Green : Colors::Red;
			Broodwar->drawCircleMap(enemyUnit->getPosition(), 
				enemyUnit->getType().groundWeapon().maxRange(),
				colorToDraw);

			CustomVector vectorToAdd(enemyUnit, currentScout);
			vectors.push_back(vectorToAdd);
		}

		for (CustomVector vec : vectors) {
			Position startPos = vec.getStartPosition();
			Position endPos = vec.getEndPosition();
			Broodwar->drawLineMap(startPos, endPos, Colors::Orange);
		}


		// Moving logic

		BWTA::Polygon enemyPoly = BWTA::getRegion(returnEnemyBaseLocs())->getPolygon();

		UnitCommand lastCommand = currentScout->getLastCommand();
		Position commandPos = lastCommand.getTargetPosition();

		
		Broodwar->drawCircleMap(commandPos, 3, Colors::Green, true);

		// is the position inside our polygon?
		bool posInsidePoly = false;
		int polyAt = -1;
		for (int i = 0; i < enemyPoly.size(); i++) {
			if (enemyPoly.at(i) == commandPos) {
				polyAt = i;
				posInsidePoly = true;
				break;
			}
		}
		// Or are we just moving somewhere else inside their base?
		if (enemyPoly.isInside(commandPos)) {
		//	posInsidePoly = true;
		}


		if (posInsidePoly) {
			// Are we moving somewhere along the edges of the polygon?
			if (polyAt != -1) {
				// Are we close enough to start moving to the next point along the polygon?
				if (currentScout->getDistance(enemyPoly.at(polyAt)) < 50) {
					// Now we need to check if can just increment
					// polyAt, or if we need to return it to 0
					if (polyAt == enemyPoly.size() - 1) {
						polyAt = 0;
					}
					else {
						polyAt++;
					}
					
					//validMove(currentScout, enemyPoly.at(polyAt));
					currentScout->move(enemyPoly.at(polyAt));
				}
			}
		}
		else {
			// Let's start moving somewhere in the polygon
			//validMove(currentScout, enemyPoly.at(0));
			currentScout->move(enemyPoly.at(0));
		}
		

		/*
		// Tell our scout to stand on the enemy chokepoint
		// so it doesn't just go and die immediately (need to implement kiting logic)
		BWTA::Chokepoint *enemyChoke = BWTA::getNearestChokepoint(returnEnemyBaseLocs());
		if (enemyChoke != NULL) {
			validMove(currentScout, enemyChoke->getCenter() + returnEnemyBaseLocs() / 128);
		}
		else {
			validMove(currentScout, returnEnemyBaseLocs());
		}
		*/



	}
}

void Scouting::recordUnitDiscover(BWAPI::UnitType u, BWAPI::TilePosition loc, int timeTick) {
	if (!u.isBuilding()) {
		return;
	}

	// If we do not already know a building
	// at this location we save it for later
	if (enemyStructures.count(loc) == 0) {
		BuildingStruct *buildStruct = new BuildingStruct();
		buildStruct->unit = u;
		buildStruct->location = loc;
		buildStruct->scoutedTime = timeTick;
		enemyStructures.insert(std::pair<BWAPI::TilePosition, BuildingStruct*>(loc, buildStruct));
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

int Scouting::getEnemyStructureCount(UnitType u) {
	int amount = 0;

	for (std::map<TilePosition, Scouting::BuildingStruct*, Scouting::CustomMapCompare>::iterator iterator = enemyStructures.begin();
		iterator != enemyStructures.end(); iterator++) {
		if (iterator->second->unit == u) {
			amount++;
		}
	}

	return amount;
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
