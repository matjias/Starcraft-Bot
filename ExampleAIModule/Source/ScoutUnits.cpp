#pragma once
#include "ScoutUnits.h"

// Most of our data types are either ScoutManager or BWAPI
using namespace BWAPI;

ScoutUnits::ScoutUnits() { }

ScoutUnits::~ScoutUnits() { }

bool ScoutUnits::addUnit(Unit unit) {
	if (!unit->exists() || !unit->isCompleted()) {
		//return false;
	}

	ScoutAndGoalStruct *newScout = new ScoutAndGoalStruct();
	newScout->scout = unit;

	return true;
}

int ScoutUnits::getAmountOfScouts() {
	return scouts.size();
}

bool ScoutUnits::assignGoal(Position goal) {
	// Has the goal already been set?
	for (auto &scout : scouts) {
		if (scout->goal = goal) {
			return false;
		}
	}

	// Otherwise we set it
	for (auto &scout : scouts) {
		if (scout->goal != Position(0, 0)) {
			scout->goal = goal;
			return true;
		}
	}

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