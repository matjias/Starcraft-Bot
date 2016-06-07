#pragma once
#include <array>
#include "BWAPI.h"
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
	void scout();

	bool addWarpingUnit(BWAPI::Unit u);
	bool recordNewUnit(BWAPI::Unit u);
	bool recordDeadUnit(BWAPI::Unit u);

private:
	StrategyName lastKnownStrategy;
	GameStage currentStage;

	void updateTacticianStart();
	void invest();

	bool defenseStructureNeeded();
	bool detectorNeeded();
	bool expansionNeeded();
	BWAPI::UnitType neededCombatUnit();
	BWAPI::UpgradeType neededUpgrade();
	void initArmyComposition();

	// Classes it holds
	ResourceSpender resourceSpender;
	UnitHandler unitHandler;

	// Class pointers
	ScoutManager* scoutManagerPtr;

	bool assigned = false;

	// Army compositions
	// VS Protoss
	std::pair<BWAPI::UnitType, float> protossEarly[2];
	std::pair<BWAPI::UnitType, float> protossMidGasLight[2];
	std::pair<BWAPI::UnitType, float> protossMidGasHeavy[1];

	// VS Terran
	std::pair<BWAPI::UnitType, float> terranEarly[2];
	std::pair<BWAPI::UnitType, float> terranMidGasLight[2];
	std::pair<BWAPI::UnitType, float> terranMidGasHeavy[1];

	// VS Zerg
	std::pair<BWAPI::UnitType, float> zergEarly[3];
	std::pair<BWAPI::UnitType, float> zergMidGasLight[4];
	std::pair<BWAPI::UnitType, float> zergMidGasHeavy[4];
};
