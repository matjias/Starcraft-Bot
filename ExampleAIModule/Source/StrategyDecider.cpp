#pragma once
#include "StrategyDecider.h"

using namespace BWAPI;

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
	else if (tacticianPtr->getBaseCount() < scoutManagerPtr->getEnemyBaseCount() ||
		tacticianPtr->mineralSurplus() ||
		(scoutManagerPtr->getEnemyDefenseValue() > 0 && 
		tacticianPtr->getBaseCount() <= scoutManagerPtr->getEnemyBaseCount())) {

		currentStrategy = Expand;
	}
	else {
		currentStrategy = Default;
	}
	
	// Draw/print
	if (currentStrategy == Default) {
		Broodwar->drawTextScreen(480, 30, "Current strategy: Default");
	}
	else if (currentStrategy == Expand) {
		Broodwar->drawTextScreen(480, 30, "Current strategy: Expand");
	}
	else if (currentStrategy == AllIn) {
		Broodwar->drawTextScreen(480, 30, "Current strategy: AllIn");
	}
	else if (currentStrategy == Defend) {
		Broodwar->drawTextScreen(480, 30, "Current strategy: Defend");
	}
}

