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
	if (!unit->exists() || !unit->isCompleted()) {
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

}

int ScoutUnits::getAmountOfScouts() {
	return scouts.size();
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
		scout->move(goal);
	}
}

std::vector<ScoutAndGoalStruct*> ScoutUnits::getScouts() {
	return scouts;
}