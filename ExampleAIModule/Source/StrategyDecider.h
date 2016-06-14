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

	void foundEnemeyBase(BWAPI::TilePosition pos);


	// Set BWTA analyzed
	void setAnalyzed();

private:
	float workerBalance();
	int defendMainBase();

	int lastStrategyUpdate = 0;

	// Class pointers
	Tactician* tacticianPtr;
	ScoutManager* scoutManagerPtr;

	// Variables
	StrategyName currentStrategy;
	bool needsScouting;
	bool mapAnalyzed = false;
};
