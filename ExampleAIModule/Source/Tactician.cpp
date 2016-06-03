#pragma once
#include "Tactician.h"

using namespace BWAPI;

Tactician::Tactician() {
	currentStage = Start;

	unitHandler._init();
	resourceSpender._init(&unitHandler);
}

Tactician::~Tactician() { }

bool Tactician::_init(ScoutManager* scoutMan) {
	if (scoutMan == NULL) {
		return false;
	}

	scoutManagerPtr = scoutMan;

	return true;
}

bool Tactician::queueUnit(Unit u){
	unitHandler.queueUnit(u);
	return true;
}

bool Tactician::recordNewUnit(Unit u) {
	// return UnitHandler.addUnit(u);
	unitHandler.addUnit(u);
	return true;
}

bool Tactician::recordDeadUnit(Unit u) {
	// return unitHandler.removeUnit(u);
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