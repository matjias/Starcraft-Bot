#pragma once
#include "ScoutUnits.h"
#include "UnitHandler.h" // Avoiding circular dependency

// Most of our data types are either ScoutManager or BWAPI
using namespace BWAPI;

ScoutUnits::ScoutUnits() { }

ScoutUnits::~ScoutUnits() { }

void ScoutUnits::setUnitHandler(UnitHandler* unitHandler) {
	unitHandlerPtr = unitHandler;
}

bool ScoutUnits::addUnit(Unit unit) {
	if (!unit->exists() || !unit->isCompleted() || unit == NULL) {
		return false;
	}

	ScoutAndGoalStruct *newScout = new ScoutAndGoalStruct();
	newScout->scout = unit;
	scouts.push_back(newScout);


	return true;
}

bool ScoutUnits::removeUnit(Unit unit) {
	bool foundUnitInScouts = false;

	int scoutPosInVector = 0;
	for (auto &scoutAndGoal : scouts) {
		if (scoutAndGoal->scout == unit) {
			foundUnitInScouts = true;
			break;
		}
		scoutPosInVector++;
	}

	if (!foundUnitInScouts) {
		return false;
	}

	scouts.erase(scouts.begin() + scoutPosInVector);
	unitHandlerPtr->addUnit(unit);
	Broodwar->sendText("Returning unit %i to unitHandler", unit->getID());
}

int ScoutUnits::getAmountOfScouts() {
	return scouts.size();
}

int ScoutUnits::getScoutCount(BWAPI::UnitType unitType) {
	int number = 0;

	for (int i = 0; i < scouts.size(); i++) {
		if (scouts[i]->scout->getType() == unitType) {
			number++;
		}
	}

	return number;
}

bool ScoutUnits::hasScouts() {
	return scouts.size() > 0;
}

bool ScoutUnits::assignGoal(Position goal) {
	// Has the goal already been set?
	for (auto &scoutAndgoal : scouts) {
		if (scoutAndgoal->goal == goal) {
			return false;
		}
	}

	// Otherwise we set it
	for (auto &scoutAndgoal : scouts) {
		if (scoutAndgoal->goal == Position(0, 0)) {
			scoutAndgoal->goal = goal;
			return true;
		}
	}

	// If we still did not set it tell the game
	Broodwar->sendText("Scout did not get a goal");

	return false;
}

void ScoutUnits::updateScouts() {
	for (auto &unit : scouts) {
		Unit scout = unit->scout;
		Position goal = unit->goal;

		// If goal has not been set we skip it
		if (goal == Position(0, 0)) {
			continue;
		}

		// @TODO:	Extend with smart moving, for
		//			example from UnitHandler / Interface
		if (!unit->peekEnemyBase) {
			scout->move(goal);
		}
		else {
			updateScoutsMoveInEnemyBase(unit);
		}
	}
}

void ScoutUnits::updateScoutsMoveInEnemyBase(ScoutAndGoalStruct* u) {
	Unit scout = u->scout;

	if (scout->isUnderAttack()) {
		u->underAttack = true;
	}

	if (!u->kiting && !u->underAttack) {
		// Go to enemy spawn base
		if (!mapAnalyzed) {
			// BWTA2 not loaded, for now just go to the spawn position
			u->scout->move(enemySpawn);

			// BWTA2 should load before scout even gets close to their spawn
			// otherwise something is somewhat wrong and it will cause
			// some unintentional behaviour
		}
		else {
			// Move scout to enemy's region
			BWTA::Region* enemyRegion = BWTA::getRegion(enemySpawn);
			BWTA::Region* scoutRegion = BWTA::getRegion(scout->getPosition());

			if (scoutRegion != enemyRegion) {
				scout->move(enemySpawn);
			}
			else {
				BWTA::Polygon regionPoly = enemyRegion->getPolygon();
				UnitCommand lastCommand = scout->getLastCommand();
				Position lastCommandPos = lastCommand.getTargetPosition();

				// Debug draw
				Broodwar->drawCircleMap(lastCommandPos, 3, Colors::Green, true);

				// Was the position inside of the polygon?
				int polyAt = -1;
				for (int i = 0; i < regionPoly.size(); i++) {
					if (regionPoly.at(i) == lastCommandPos) {
						polyAt = i;
						break;
					}
				}

				if (polyAt == -1) {
					// If we did not move alongside the polygon
					scout->move(regionPoly.at(0));
				}
				else {
					if (scout->getDistance(regionPoly.at(polyAt)) >=
						scout->getType().sightRange()) {
						
						// Do not update any commands, the scout is
						// moving exactly as we want it to
						return;
					}

					int nextPoly = (polyAt + 1) % regionPoly.size();
					int nextPoly2 = (polyAt + 2) % regionPoly.size();

					// Edge case: Is the next point on the polygon in a spike?
					// That is, would it just be easier to move on to the next point
					// on the polygon.
					if (regionPoly.at(polyAt).getDistance(regionPoly.at(nextPoly2)) <
						regionPoly.at(polyAt).getDistance(regionPoly.at(nextPoly))) {

						scout->move(regionPoly.at(nextPoly2));
					}
					else {
						scout->move(regionPoly.at(nextPoly));
					}
				}
			}
		}
	}
	else {
		// more stoofs

		// For now just 'saves' the drone by returning it to UnitHandler
		removeUnit(scout);
	}
}

std::vector<ScoutAndGoalStruct*> ScoutUnits::getScouts() {
	return scouts;
}

void ScoutUnits::setAnalyzed() {
	mapAnalyzed = true;
}

void ScoutUnits::setEnemySpawn(TilePosition pos) {
	enemySpawn = Position(pos);
}