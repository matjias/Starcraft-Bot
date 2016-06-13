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
	decideStrategy();

	if (needsScouting) {
		tacticianPtr->scout();
	}

	tacticianPtr->updateTactician(currentStrategy);
	scoutManagerPtr->updateScoutManager();

}

void StrategyDecider::decideStrategy() {
	/*if (needsToUpdateStrategy) {
		currentStrategy = Default;
		needsToUpdateStrategy = false;
	}*/

	if (false) { // @TODO: Enemies in our regions
		currentStrategy = Defend;
	}
	else if (false) { // @TODO: Behind in expansions && behind in worker count && behind in army value
		currentStrategy = AllIn;
	}
	else if (false) { // @TODO: Behind in expansions || has mineral surplus || enemy builds defenses
		currentStrategy = Expand;
	}
	else {
		currentStrategy = Default;
	}
}

