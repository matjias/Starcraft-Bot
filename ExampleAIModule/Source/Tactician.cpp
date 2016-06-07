#pragma once
#include "Tactician.h"

using namespace BWAPI;

Tactician::Tactician() {
	currentStage = Start;

	initArmyComposition();

	unitHandler._init();
	resourceSpender._init(&unitHandler, unitHandler.getBuildingUnits(), unitHandler.getProbeUnits());
}

Tactician::~Tactician() { }

bool Tactician::_init(ScoutManager* scoutMan) {
	if (scoutMan == NULL) {
		return false;
	}

	scoutManagerPtr = scoutMan;

	return true;
}

bool Tactician::addWarpingUnit(Unit u){
	unitHandler.addWarpingUnit(u);
	return true;
}

bool Tactician::recordNewUnit(Unit u) {
	// return UnitHandler.addUnit(u);
	unitHandler.addUnit(u);
	return true;
}

bool Tactician::recordDeadUnit(Unit u) {
	// return unitHandler.removeUnit(u);
	return false;
}

void Tactician::scout() {
	// Hack
	if (!assigned) {
		unitHandler.addScout(BWAPI::UnitTypes::Protoss_Probe);
		scoutManagerPtr->beginScouting(unitHandler.getScoutUnits());
		assigned = true;
	}
	
}

void Tactician::updateTactician(StrategyName currentStategy) {
	if (lastKnownStrategy != currentStategy) {
		// Some switching logic?

		lastKnownStrategy = currentStategy;
	}

	switch (currentStage) {
	case Start:
		updateTacticianStart();
		break;

	case Early:


		break;

	case Mid:


		break;
	}
	
}

void Tactician::updateTacticianStart() {
	if (lastKnownStrategy == Defend) {
		// Update defend units

	}

	// UnitHandler.mineMinerals()
	
	unitHandler.update();

}

void Tactician::invest() {
	// @TODO
	if (lastKnownStrategy == Default) {
		resourceSpender.addUnitInvestment(BWAPI::UnitTypes::Protoss_Zealot, false);
	}
	else {
		resourceSpender.addUnitInvestment(BWAPI::UnitTypes::Protoss_Dragoon, false);
	}
}

bool Tactician::defenseStructureNeeded() {
	// @TODO
	return false;
}

bool Tactician::detectorNeeded() {
	// @TODO
	return false;
}

bool Tactician::expansionNeeded() {
	// @TODO
	return lastKnownStrategy == Expand;
}

BWAPI::UnitType Tactician::neededCombatUnit() {
	// @TODO
	return NULL;
}

BWAPI::UpgradeType Tactician::neededUpgrade() {
	// @TODO
	return NULL;
}

void Tactician::initArmyComposition() {
	// Army compositions
	// VS Protoss
	protossEarly[0] = std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0);
	protossEarly[1] = std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.5);
	protossMidGasLight[0] = std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0);
	protossMidGasLight[1] = std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.5);
	protossMidGasHeavy[0] = std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0);

	// VS Terran
	terranEarly[0] = std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0);
	terranEarly[1] = std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.5);
	terranMidGasLight[0] = std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0);
	terranMidGasLight[1] = std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.5);
	terranMidGasHeavy[0] = std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0);

	// VS Zerg
	zergEarly[0] = std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0);
	zergEarly[2] = std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.2);
	zergEarly[3] = std::make_pair(BWAPI::UnitTypes::Protoss_Corsair, 0.2);
	zergMidGasLight[0] = std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0);
	zergMidGasLight[1] = std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.1);
	zergMidGasLight[2] = std::make_pair(BWAPI::UnitTypes::Protoss_Corsair, 0.1);
	zergMidGasLight[3] = std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 0.1);
	zergMidGasHeavy[0] = std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0);
	zergMidGasHeavy[1] = std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0);
	zergMidGasHeavy[2] = std::make_pair(BWAPI::UnitTypes::Protoss_Corsair, 0.5);
	zergMidGasHeavy[3] = std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 1.0);
}
