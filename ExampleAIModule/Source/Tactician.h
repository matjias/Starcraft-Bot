#pragma once
#include <BWAPI.h>
#include "Constants.h"
#include "ResourceSpender.h"
#include "UnitHandler.h"
#include "ScoutManager.h"

class Tactician {
public:
	Tactician();
	~Tactician();
	
	bool _init(ScoutManager* scoutMan);

	void updateTactician(StrategyName currentStrategy);

	bool recordNewUnit(BWAPI::Unit u);
	bool recordDeadUnit(BWAPI::Unit u);

private:
	StrategyName lastKnownStrategy;
	GameStage currentStage;

	void updateTacticianStart();

	// Classes it holds
	ResourceSpender resourceSpender;
	UnitHandler unitHandler;

	// Class pointers
	ScoutManager* scoutManagerPtr;

};
