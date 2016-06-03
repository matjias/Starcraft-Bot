#pragma once
#include "StrategyDecider.h"

StrategyDecider::StrategyDecider() {
	currentStrategy = Default;
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
	tacticianPtr->updateTactician(currentStrategy);

}


void StrategyDecider::decideStrategy() {
	if (needsToUpdateStrategy) {
		currentStrategy = Default;
		needsToUpdateStrategy = false;
	}
}

