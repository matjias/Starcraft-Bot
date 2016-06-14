#pragma once
#include "StrategyDecider.h"

using namespace BWAPI;

StrategyDecider::StrategyDecider() {
	currentStrategy = Default;
	strategyUpdateTime = STRATEGY_UPDATE_TIME;
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
	strategyUpdateTime++;

	if (strategyUpdateTime >= STRATEGY_UPDATE_TIME) {
		decideStrategy();
		strategyUpdateTime = 0;
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

	if (false) { // @TODO: Enemy combat units in our regions
		currentStrategy = Defend;
	}
	else if (scoutManagerPtr->getEnemyDefenseValue() == 0 &&
		tacticianPtr->getBaseCount() < scoutManagerPtr->getEnemyBaseCount() &&
		workerBalance() <= ALLIN_WORKER_BALANCE) {
		
		currentStrategy = AllIn;
	}
	else if (tacticianPtr->getBaseCount() < scoutManagerPtr->getEnemyBaseCount() ||
		(scoutManagerPtr->getEnemyDefenseValue() > 0 && 
		tacticianPtr->getBaseCount() <= scoutManagerPtr->getEnemyBaseCount())) {

		currentStrategy = Expand;
	}
	else {
		currentStrategy = Default;
	}
}

float StrategyDecider::workerBalance() {
	if (tacticianPtr->getWorkerCount() + scoutManagerPtr->getEnemyWorkerCount() > 0) {
		return tacticianPtr->getWorkerCount() / 
			(tacticianPtr->getWorkerCount() + scoutManagerPtr->getEnemyWorkerCount());
	}
	else {
		return DEFAULT_WORKER_BALANCE;
	}
}
