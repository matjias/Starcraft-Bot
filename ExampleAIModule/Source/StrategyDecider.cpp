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
	if (redecideStrategy ||
		lastStrategyUpdate + STRATEGY_UPDATE_TIME < Broodwar->getFrameCount()) {

		lastStrategyUpdate = Broodwar->getFrameCount();
		decideStrategy();
		redecideStrategy = false;
	}

	/*if (lastStrategyUpdate + STRATEGY_UPDATE_TIME < Broodwar->getFrameCount()) {
		lastStrategyUpdate = Broodwar->getFrameCount();
		
	}*/

	if (needsScouting) {
		tacticianPtr->scout();
	}

	tacticianPtr->updateTactician(currentStrategy);
	scoutManagerPtr->updateScoutManager();

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
	else {
		Broodwar->drawTextScreen(480, 30, "Current strategy: ");
	}
}

void StrategyDecider::updateStrategy() {
	redecideStrategy = true;
}

void StrategyDecider::decideStrategy() {
	int defendMainBaseResult = defendMainBase();
	if (defendMainBaseResult > 0) { // @TODO: Enemy combat units in our regions
		currentStrategy = Defend;
		if (defendMainBaseResult > 1) {
			//Broodwar->sendText("Enemies in base");
		}
		else {
			//Broodwar->sendText("Enemy in base");
		}
	}
	else if (scoutManagerPtr->getEnemyDefenseValue() == 0 &&
		tacticianPtr->getBaseCount() < scoutManagerPtr->getEnemyBaseCount() &&
		workerBalance() <= ALLIN_WORKER_BALANCE) {
		
		currentStrategy = AllIn;
	}
	else if (tacticianPtr->getBuildExpansions() &&
		(tacticianPtr->getBaseCount() < scoutManagerPtr->getEnemyBaseCount() ||
		(scoutManagerPtr->getEnemyDefenseValue() > 0 && 
		tacticianPtr->getBaseCount() <= scoutManagerPtr->getEnemyBaseCount()))) {

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

int StrategyDecider::defendMainBase() {
	if (!mapAnalyzed) {
		return false;
	}

	Unitset visisbleEnemyUnits = Broodwar->enemy()->getUnits();
	int enemiesInOurBase = 0;

	BWTA::Region* ourRegion = BWTA::getRegion(Broodwar->self()->getStartLocation());
	for (auto &u : visisbleEnemyUnits) {
		BWTA::Region* unitRegion = BWTA::getRegion(u->getPosition());

		if (ourRegion == unitRegion) {
			enemiesInOurBase++;
			
			if (enemiesInOurBase > 1) {
				break;
			}
		}
	}

	return enemiesInOurBase;
}

void StrategyDecider::setAnalyzed() {
	tacticianPtr->setAnalyzed();
	mapAnalyzed = true;
}

void StrategyDecider::foundEnemeyBase(TilePosition pos) {
	tacticianPtr->foundEnemyBase(pos);
}