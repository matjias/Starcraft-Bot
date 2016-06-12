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
	
	// Only used in Unit Testing!!!
	void setBroodwarMock(BWAPI::Game* broodwarPtr);

	bool _init(ScoutManager* scoutMan);
	
	

	void updateTactician(StrategyName currentStrategy);
	void scout();

	bool addWarpingUnit(BWAPI::Unit u);
	bool recordNewUnit(BWAPI::Unit u);
	bool recordDeadUnit(BWAPI::Unit u);

	void setAnalyzed();

	void addAllScouts();

private:
	StrategyName lastKnownStrategy;
	GameStage currentStage;
	GameStage previousStage;

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
	void setStage();

	// Classes it holds
	ResourceSpender resourceSpender;
	UnitHandler unitHandler;

	// Class pointers
	ScoutManager* scoutManagerPtr;

	bool assigned = false;

	// Army compositions vs Protoss, Terran, and Zerg
	std::vector<std::pair<BWAPI::UnitType, float>> armyComposition;

	std::vector<std::pair<BWAPI::UnitType, float>> initialArmyComposition;

	std::vector<std::pair<BWAPI::UnitType, float>> protossEarlyGasLight;
	std::vector<std::pair<BWAPI::UnitType, float>> protossEarlyGasHeavy;
	std::vector<std::pair<BWAPI::UnitType, float>> protossMidGasLight;
	std::vector<std::pair<BWAPI::UnitType, float>> protossMidGasHeavy;

	std::vector<std::pair<BWAPI::UnitType, float>> terranEarlyGasLight;
	std::vector<std::pair<BWAPI::UnitType, float>> terranEarlyGasHeavy;
	std::vector<std::pair<BWAPI::UnitType, float>> terranMidGasLight;
	std::vector<std::pair<BWAPI::UnitType, float>> terranMidGasHeavy;

	std::vector<std::pair<BWAPI::UnitType, float>> zergEarlyGasLight;
	std::vector<std::pair<BWAPI::UnitType, float>> zergEarlyGasHeavy;
	std::vector<std::pair<BWAPI::UnitType, float>> zergMidGasLight;
	std::vector<std::pair<BWAPI::UnitType, float>> zergMidGasHeavy;

	std::vector<std::pair<BWAPI::UnitType, float>> dummyArmyComposition;
	bool useDummyArmyComposition;
};
