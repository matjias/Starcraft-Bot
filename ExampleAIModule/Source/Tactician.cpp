#pragma once
#include "Tactician.h"

using namespace BWAPI;

Tactician::Tactician() {
	currentStage = Start;

	UnitHandler._init();
}

Tactician::~Tactician() { }

bool Tactician::_init(ScoutManager* scoutMan) {
	if (scoutMan == NULL) {
		return false;
	}

	ScoutManagerPtr = scoutMan;

	return true;
}

bool Tactician::recordNewUnit(Unit u) {
	// return UnitHandler.addUnit(u);
	UnitHandler.addUnit(u);
	return true;
}

bool Tactician::recordDeadUnit(Unit u) {
	// return UnitHandler.removeUnit(u);
	return false;
}

void Tactician::updateTactician(StrategyName currentStategy) {
	if (lastKnownStrategy != currentStategy) {
		// Some switching logic?

		lastKnownStrategy = currentStategy;
	}

	switch (currentStage) {
	case Start:
		updateTacticianStart();
		break;

	case Early:


		break;

	case Mid:


		break;
	}
	
}

void Tactician::updateTacticianStart() {
	if (lastKnownStrategy == Defend) {
		// Update defend units

	}

	// UnitHandler.mineMinerals()


}