#pragma once
#include "Tactician.h"

using namespace BWAPI;

Tactician::Tactician() { }

Tactician::~Tactician() { }

bool Tactician::_init(ScoutManager* scoutMan) {

	currentStage = Start;

	initArmyCompositions();
	armyComposition = initialArmyComposition;

	resourceSpender._init(&unitHandler, unitHandler.getBuildingUnits(), unitHandler.getProbeUnits());



	if (scoutMan == NULL) {
		return false;
	}

	scoutManagerPtr = scoutMan;

	return true;
}

void Tactician::setBroodwarMock(Game* broodwarPtr) {
	unitHandler.setBroodwarMock(broodwarPtr);
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

	/*bool moveBol = u->move(Position(Broodwar->self()->getStartLocation()));
	Unit uu = u->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Nexus);


	Broodwar->sendText("Move: %i", moveBol ? 1 : 0);
	Broodwar->sendText("Unit was: %s", u->getType().c_str());
	Broodwar->sendText("Pos: %i, %i", u->getPosition().x, u->getPosition().y);

	if (uu == NULL) {
		Broodwar->sendText("Closest unit was null");
	}
	else {
		Broodwar->sendText("Closest unit was: %s", uu->getType().c_str());
	}*/


	// return unitHandler.removeUnit(u);
	return unitHandler.deleteUnit(u);
	return false;
}

void Tactician::scout() {
	// Hack
	if (!assigned) {
		assigned = unitHandler.addScout(BWAPI::UnitTypes::Protoss_Probe);
		if (assigned){
			scoutManagerPtr->beginScouting(unitHandler.getScoutUnits());
		}
	}
	
}

void Tactician::addAllScouts() {
	while (scoutManagerPtr->canAddAnotherScout()) {
		unitHandler.addScout(BWAPI::UnitTypes::Protoss_Probe);
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
	
	invest();
	resourceSpender.update();
}

void Tactician::updateTacticianStart() {
	if (lastKnownStrategy == Defend) {
		// Update defend units

	}
	
	unitHandler.update();

}

void Tactician::setAnalyzed(){
	unitHandler.setAnalyzed();
}

void Tactician::invest() {
	
	if (detectorNeeded()) {
		resourceSpender.addUnitInvestment(BWAPI::UnitTypes::Protoss_Observer, true);
	}
	else if (defenseStructureNeeded()) {
		resourceSpender.addUnitInvestment(BWAPI::UnitTypes::Protoss_Photon_Cannon, false);
	}
	else if (expansionNeeded()) {
		resourceSpender.addUnitInvestment(BWAPI::UnitTypes::Protoss_Nexus, false);
}
	else if (neededUpgrade() != NULL) {
		resourceSpender.addUpgradeInvestment(neededUpgrade(), false);
	}
	else if (neededCombatUnit() != NULL) {
		resourceSpender.addUnitInvestment(neededCombatUnit(), false);
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
	if (armyComposition.size() == 1) {
		return armyComposition[0].first;
	}
	else if (armyComposition.size() > 1) {

		float minRate = (unitHandler.getCombatUnits()->getUnitCount(armyComposition[0].first) +
			unitHandler.getWarpingUnitCount(armyComposition[0].first)) /
			armyComposition[0].second;
		int minPosition = 0;

		// Find the unit type that there are fewest of compared to the how many there should be
		for (int i = 1; i < armyComposition.size(); i++) {
			if (armyComposition[i].second > 0 &&
				(unitHandler.getCombatUnits()->getUnitCount(armyComposition[i].first) +
				unitHandler.getWarpingUnitCount(armyComposition[i].first)) /
				armyComposition[i].second < minRate) {

				minRate = (unitHandler.getCombatUnits()->getUnitCount(armyComposition[i].first) +
					unitHandler.getWarpingUnitCount(armyComposition[i].first)) /
					armyComposition[i].second;
				minPosition = i;
			}
		}

		return armyComposition[minPosition].first;
	}

	return NULL;
}

BWAPI::UpgradeType Tactician::neededUpgrade() {
	// @TODO
	return NULL;
}

bool Tactician::mineralSurplus() {
	return Broodwar->self()->minerals() > MINERAL_SURPLUS_LIMIT;
}

bool Tactician::gasSurplus() {
	return Broodwar->self()->gas() > GAS_SURPLUS_LIMIT;
}

void Tactician::computeArmyComposition() {

	switch (currentStage) {
	case Start:
		armyComposition = initialArmyComposition;
		break;

	case Early:
		if (Broodwar->self()->getRace() == BWAPI::Races::Zerg) {
			if (currentStage == Mid) {
				if (gasSurplus()) {
					armyComposition = zergEarlyGasHeavy;
				}
				else if (mineralSurplus()) {
					armyComposition = zergEarlyGasLight;
				}
			}
		}
		else if (Broodwar->self()->getRace() == BWAPI::Races::Terran) {
			if (currentStage == Mid) {
				if (gasSurplus()) {
					armyComposition = terranEarlyGasHeavy;
				}
				else if (mineralSurplus()) {
					armyComposition = terranEarlyGasLight;
				}
			}
		}
		else if (Broodwar->self()->getRace() == BWAPI::Races::Protoss) {
			if (currentStage == Mid) {
				if (gasSurplus()) {
					armyComposition = protossEarlyGasHeavy;
				}
				else if (mineralSurplus()) {
					armyComposition = protossEarlyGasLight;
				}
			}
		}
		break;

	case Mid:
		if (Broodwar->self()->getRace() == BWAPI::Races::Zerg) {
			if (currentStage == Mid) {
				if (gasSurplus()) {
					armyComposition = zergMidGasHeavy;
				}
				else if (mineralSurplus()) {
					armyComposition = zergMidGasLight;
				}
			}
		}
		else if (Broodwar->self()->getRace() == BWAPI::Races::Terran) {
			if (currentStage == Mid) {
				if (gasSurplus()) {
					armyComposition = terranMidGasHeavy;
				}
				else if (mineralSurplus()) {
					armyComposition = terranMidGasLight;
				}
			}
		}
		else if (Broodwar->self()->getRace() == BWAPI::Races::Protoss) {
			if (currentStage == Mid) {
				if (gasSurplus()) {
					armyComposition = protossMidGasHeavy;
				}
				else if (mineralSurplus()) {
					armyComposition = protossMidGasLight;
				}
			}
		}
		break;
	}
}

void Tactician::initArmyCompositions() {

	// Army compositions vs Protoss, Terran, and Zerg
	initialArmyComposition.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));

	protossEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	protossEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.5));
	protossEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
	protossMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	protossMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
	protossMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
	protossMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 0.5));

	terranEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	terranEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.2));
	terranEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	terranEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 5.0));
	terranMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	terranMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
	terranMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
	terranMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 0.5));

	zergEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	zergEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.2));
	zergEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Corsair, 0.1));
	zergEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	zergEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
	zergEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Corsair, 0.2));
	zergMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	zergMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.1));
	zergMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Corsair, 0.1));
	zergMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 0.1));
	zergMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
	zergMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Corsair, 0.2));
	zergMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 0.5));
}