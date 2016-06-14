#pragma once
#include "BWAPI.h"
#include "Constants.h"
#include "ScoutManager.h"
#include "Tactician.h"

class StrategyDecider {
public:
	StrategyDecider();
	~StrategyDecider();

	bool _init(Tactician* tact, ScoutManager* scoutMan);

	// On frame
	void update();

	bool needsToUpdateStrategy = true;
	
	void decideStrategy();

private:
	float workerBalance();

	int strategyUpdateTime;

	// Class pointers
	Tactician* tacticianPtr;
	ScoutManager* scoutManagerPtr;

	// Variables
	StrategyName currentStrategy;
	bool needsScouting;
};
