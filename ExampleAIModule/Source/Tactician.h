#pragma once
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

	void setAnalyzed(bool analyzed);

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
	bool mineralSurplus();
	bool gasSurplus();
	void initArmyCompositions();
	void computeArmyComposition();

	// Classes it holds
	ResourceSpender resourceSpender;
	UnitHandler unitHandler;

	// Class pointers
	ScoutManager* scoutManagerPtr;

	bool assigned = false;

	// Army compositions vs Protoss, Terran, and Zerg
	std::vector<std::pair<BWAPI::UnitType, float>> armyComposition;

	std::vector<std::pair<BWAPI::UnitType, float>> initialArmyComposition;

	std::vector<std::pair<BWAPI::UnitType, float>> protossEarly;
	std::vector<std::pair<BWAPI::UnitType, float>> protossMidGasLight;
	std::vector<std::pair<BWAPI::UnitType, float>> protossMidGasHeavy;

	std::vector<std::pair<BWAPI::UnitType, float>> terranEarly;
	std::vector<std::pair<BWAPI::UnitType, float>> terranMidGasLight;
	std::vector<std::pair<BWAPI::UnitType, float>> terranMidGasHeavy;

	std::vector<std::pair<BWAPI::UnitType, float>> zergEarly;
	std::vector<std::pair<BWAPI::UnitType, float>> zergMidGasLight;
	std::vector<std::pair<BWAPI::UnitType, float>> zergMidGasHeavy;
};
