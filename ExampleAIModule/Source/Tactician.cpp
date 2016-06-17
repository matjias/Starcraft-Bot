#pragma once
#include "Tactician.h"

using namespace BWAPI;

Tactician::Tactician() { }

Tactician::~Tactician() { }

bool Tactician::_init(ScoutManager* scoutMan) {

	currentStage = Start;
	previousStage = currentStage;

	// AI settings
	useDummyArmyComposition = false;
	buildDetectors = true;
	buildDefenseStructures = true;
	buildExpansions = false;
	researchUpgrades = true;
	buildCombatUnits = true;

	initArmyCompositions();
	computeArmyComposition();

	resourceSpender._init(&unitHandler, unitHandler.getBuildingUnits(), unitHandler.getProbeUnits());

	if (scoutMan == NULL) {
		return false;
	}

	scoutManagerPtr = scoutMan;
	armyBalance = DEFAULT_ARMY_BALANCE;

	return true;
}

void Tactician::setBroodwarMock(Game* broodwarPtr) {
	unitHandler.setBroodwarMock(broodwarPtr);
}

bool Tactician::addWarpingUnit(Unit u){
	unitHandler.addWarpingUnit(u);
	resourceSpender.removePlacedBuildings(u->getType());
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

void Tactician::updateTactician(StrategyName currentStrategy) {
	computeArmyBalance();

	if (lastKnownStrategy != currentStrategy) {
		resourceSpender.setStrategy(currentStrategy);
		lastKnownStrategy = currentStrategy;
	}

	previousStage = currentStage;
	setStage();
	if (!useDummyArmyComposition) {
		computeArmyComposition();
	}

	unitHandler.update();
	updateTacticianStart();
	
	switch (currentStage) {
	case Start:

		break;

	case Early:

		break;

	case Mid:


		break;
	}
	
	invest();
	resourceSpender.update();

	// Draw/print
	//Broodwar->drawTextScreen(480, 30, "Current stage: %i", currentStage);
}

void Tactician::updateTacticianStart() {
	if (lastKnownStrategy == Defend) {
		// Update defend units

	}
	if (mapAnalyzed){
		//if (unitHandler.getCombatUnits()->getUnitCount(UnitTypes::Protoss_Dragoon) > 8) {
		if (armyBalance > ARMY_ATTACK_POWER_BALANCE) {
			// @TODO: Attack move to nearest building instead
			attack = true;
			unitHandler.getCombatUnits()->runAttack(Position(scoutManagerPtr->getEnemySpawn()));
		}
		else if (armyBalance < ARMY_RETREAT_POWER_BALANCE) {
			// @TODO: Move to home
			unitHandler.getCombatUnits()->runAttack(rendezvousPos);
		}
	}
	
	Broodwar->drawTextScreen(480, 40, "Army Balance: %f", armyBalance);
	Broodwar->drawTextScreen(480, 50, "Zealot Count: %i", unitHandler.getCombatUnits()->getUnitCount(UnitTypes::Protoss_Zealot));
	Broodwar->drawTextScreen(480, 60, "Dragoo Count: %i", unitHandler.getCombatUnits()->getUnitCount(UnitTypes::Protoss_Dragoon));
	Broodwar->drawTextScreen(480, 70, "worke Count: %i", unitHandler.getProbeUnits()->getWorkerCount());
	Broodwar->drawTextScreen(480, 80, "Scout Count: %i", unitHandler.getScoutUnits()->getAmountOfScouts());
	Broodwar->drawTextScreen(480, 90, "pylon Count: %i", unitHandler.getBuildingUnits()->getBuildingCount(UnitTypes::Protoss_Pylon));
	

}

void Tactician::setStage() {
	switch (currentStage) {
	case Start:
		if (Broodwar->self()->supplyUsed() / 2 >= EARLY_STAGE_SUPPLY) {
			currentStage = Early;
		}
		break;

	case Early:
		if (Broodwar->self()->supplyUsed() / 2 >= MID_STAGE_SUPPLY) {
			currentStage = Mid;
		}
		break;

	case Mid:


		break;
	}
}

void Tactician::setAnalyzed(){
	unitHandler.setAnalyzed();
	mapAnalyzed = true;

	// Calculate Rendezvous Point
	BWTA::Region* ownRegion = BWTA::getRegion(Broodwar->self()->getStartLocation());
	std::set<BWTA::Chokepoint*> chokesFromOwn = ownRegion->getChokepoints();

	if (chokesFromOwn.size() > 1) {
		Broodwar->sendText("Multiple chokes in from our region");
	}
	else {
		BWTA::Chokepoint* chokeInOwn = *chokesFromOwn.begin();
		std::pair<BWTA::Region*, BWTA::Region*> regs = chokeInOwn->getRegions();
		BWTA::Region* neighborRegion = regs.first == ownRegion ?
			neighborRegion = regs.second :
			neighborRegion = regs.first;

		rendezvousPos = neighborRegion->getCenter();
	}
	unitHandler.getCombatUnits()->setAnalyzed(rendezvousPos);
}

void Tactician::invest() {
	if (buildDetectors && detectorNeeded()) {
		resourceSpender.addUnitInvestment(BWAPI::UnitTypes::Protoss_Observer, true);
	}
	else if (buildDefenseStructures && defenseStructureNeeded()) {
		resourceSpender.addUnitInvestment(BWAPI::UnitTypes::Protoss_Photon_Cannon, false);
	}
	else if (buildExpansions && expansionNeeded()) {
		resourceSpender.addUnitInvestment(BWAPI::UnitTypes::Protoss_Nexus, false);
	}
	else if (researchUpgrades && neededUpgrade() != NULL) {
		resourceSpender.addUpgradeInvestment(neededUpgrade(), false);
	}
	else if (buildCombatUnits && neededCombatUnit() != NULL) {
		resourceSpender.addUnitInvestment(neededCombatUnit(), false);
	}
}

bool Tactician::defenseStructureNeeded() {
	// @TODO
	return false;
}

bool Tactician::detectorNeeded() {
	if (!detectionNeeded && enemyCloakPossible()) {
		detectionNeeded = true;
	}

	return unitHandler.getWarpingUnitCount(UnitTypes::Protoss_Observer) +
		unitHandler.getScoutUnits()->getScoutCount(UnitTypes::Protoss_Observer) +
		unitHandler.getCombatUnits()->getUnitCount(UnitTypes::Protoss_Observer) <
		neededDetectors();
}

int Tactician::neededDetectors() {
	int number = 0;

	if (currentStage == Mid || currentStage == Late) {
		number += OBSERVERS_TO_SCOUT;
	}

	if (detectionNeeded) {
		number += OBSERVERS_WITH_ARMY;
	}

	return number;
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

		if (resourceSpender.getUnitWithPendingTech() == NULL) {

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
		else {
			// Add an alternate unit if the pending unit requires more tech
			if (resourceSpender.getUnitWithPendingTech() == armyComposition[0].first) {
				return armyComposition[armyComposition.size() - 1].first;
			}
			else {
				for (int i = 1; i < armyComposition.size(); i++) {
					if (resourceSpender.getUnitWithPendingTech() == armyComposition[i].first) {
						return armyComposition[i - 1].first;
					}
				}
			}
		}
	}

	return NULL;
}

int Tactician::getBaseCount() {
	return unitHandler.getWarpingUnitCount(UnitTypes::Protoss_Nexus) +
		unitHandler.getBuildingUnits()->getBuildingCount(UnitTypes::Protoss_Nexus);
}
int Tactician::getWorkerCount() {
	return unitHandler.getWarpingUnitCount(UnitTypes::Protoss_Probe) + 
		unitHandler.getProbeUnits()->getWorkerCount() + 
		unitHandler.getScoutUnits()->getScoutCount(UnitTypes::Protoss_Probe) + 
		unitHandler.getCombatUnits()->getUnitCount(UnitTypes::Protoss_Probe);
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

void Tactician::computeArmyBalance() {
	if (unitHandler.getArmyValue() + scoutManagerPtr->getEnemyArmyValue() > 0) {
		armyBalance = unitHandler.getArmyValue() / 
			(unitHandler.getArmyValue() + scoutManagerPtr->getEnemyArmyValue());
	}
}

void Tactician::computeArmyComposition() {

	if (useDummyArmyComposition) {
		armyComposition = dummyArmyComposition;
	}
	else {
		switch (currentStage) {
		case Start:
			armyComposition = initialArmyComposition;
			break;

		case Early:
			if (Broodwar->enemy()->getRace() == BWAPI::Races::Zerg) {
				if (mineralSurplus() && !gasSurplus()) {
					armyComposition = zergEarlyGasLight;
				}
				else {
					armyComposition = zergEarlyGasHeavy;
				}
			}
			else if (Broodwar->enemy()->getRace() == BWAPI::Races::Terran) {
				if (mineralSurplus() && !gasSurplus()) {
					armyComposition = terranEarlyGasLight;
				}
				else {
					armyComposition = terranEarlyGasHeavy;
				}
			}
			else {
				if (mineralSurplus() && !gasSurplus()) {
					armyComposition = protossEarlyGasLight;
				}
				else {
					armyComposition = protossEarlyGasHeavy;
				}
			}
			break;

		case Mid:
			if (Broodwar->enemy()->getRace() == BWAPI::Races::Zerg) {
				if (mineralSurplus() && !gasSurplus()) {
					armyComposition = zergMidGasLight;
				}
				else {
					armyComposition = zergMidGasHeavy;
				}
			}
			else if (Broodwar->enemy()->getRace() == BWAPI::Races::Terran) {
				if (mineralSurplus() && !gasSurplus()) {
					armyComposition = terranMidGasLight;
				}
				else {
					armyComposition = terranMidGasHeavy;
				}
			}
			else {
				if (mineralSurplus() && !gasSurplus()) {
					armyComposition = protossMidGasLight;
				}
				else {
					armyComposition = protossMidGasHeavy;
				}
			}
			break;
		}
	}
}

void Tactician::initArmyCompositions() {

	// Army compositions vs Protoss, Terran, and Zerg
	initialArmyComposition.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));

	protossEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	//protossEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.5));
	//protossEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	protossEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 100.0));
	protossMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	protossMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.5));
	protossMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 0.01));
	protossMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	protossMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 100.0));
	protossMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 50.0));

	terranEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	terranEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.1));
	terranEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	terranEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 100.0));
	terranMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	terranMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.5));
	terranMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 0.1));
	terranMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	terranMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 100.0));
	terranMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 100.0));

	zergEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	zergEarlyGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.1));
	zergEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	zergEarlyGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
	zergMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	zergMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 0.1));
	zergMidGasLight.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 0.01));
	zergMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	zergMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
	zergMidGasHeavy.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_High_Templar, 1.0));

	//dummyArmyComposition.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Zealot, 1.0));
	dummyArmyComposition.push_back(std::make_pair(BWAPI::UnitTypes::Protoss_Dragoon, 1.0));
}

bool Tactician::enemyCloakPossible() {
	// Detection is needed if the enemy has a unit that can cloak,
	// a unit from a similar tech path, or the building unlocking/constructing them
	return scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Citadel_of_Adun) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Templar_Archives) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Robotics_Facility) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Observatory) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Arbiter_Tribunal) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_High_Templar) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Archon) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Dark_Templar) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Dark_Archon) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Observer) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Shuttle) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Reaver) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Protoss_Arbiter) ||

		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Nuclear_Silo) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Factory) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Machine_Shop) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Starport) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Control_Tower) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Science_Facility) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Covert_Ops) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Physics_Lab) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Ghost) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Nuclear_Missile) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Vulture) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Vulture_Spider_Mine) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Goliath) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Wraith) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Dropship) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Valkyrie) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Science_Vessel) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Terran_Battlecruiser) ||

		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Zerg_Hydralisk_Den) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Zerg_Hydralisk) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Zerg_Lurker_Egg) ||
		scoutManagerPtr->getAmountOfEnemyUnit(BWAPI::UnitTypes::Zerg_Lurker);
}

Position Tactician::getRendezvousPos() {
	return rendezvousPos;
}

void Tactician::foundEnemyBase(TilePosition pos) {
	tilePathToEnemy = BWTA::getShortestPath(TilePosition(rendezvousPos), pos);
}

std::vector<BWAPI::TilePosition> Tactician::getPathToEnemy() {
	return tilePathToEnemy;
}

bool Tactician::getBuildExpansions() {
	return buildExpansions;
}
