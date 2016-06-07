#pragma once
#include "StrategyDecider.h"

StrategyDecider::StrategyDecider() {
	currentStrategy = Default;
	needsScouting = true;
}

StrategyDecider::~StrategyDecider() { }

bool StrategyDecider::_init(Tactician* tact, ScoutManager* scoutMan) {
	if (tact == NULL || scoutMan == NULL) {
		return false;
	}

	tacticianPtr = tact;
	scoutManagerPtr = scoutMan;

	return true;
}

void StrategyDecider::update() {
	if (needsScouting) {
		tacticianPtr->scout();
	}

	tacticianPtr->updateTactician(currentStrategy);
	scoutManagerPtr->updateScoutManager();

}


void StrategyDecider::decideStrategy() {
	if (needsToUpdateStrategy) {
		currentStrategy = Default;
		needsToUpdateStrategy = false;
	}
}

