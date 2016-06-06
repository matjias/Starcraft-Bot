#pragma once
#include "ResourceSpender.h"

using namespace BWAPI;

ResourceSpender::ResourceSpender() { }

ResourceSpender::~ResourceSpender() { }

bool ResourceSpender::_init(UnitHandler* unitHandler, BuildingUnits* buildingUnits, ProbeUnits* probeUnits) {
	if (unitHandler == NULL || buildingUnits == NULL || probeUnits == NULL) {
		return false;
	}

	unitHandlerPtr = unitHandler;
	buildingUnitsPtr = buildingUnits;
	probeUnitsPtr = probeUnits;

	return true;
}

UnitOrUpgrade::UnitOrUpgrade(BWAPI::UnitType unitType) {
	// @TODO
}

UnitOrUpgrade::UnitOrUpgrade(BWAPI::UpgradeType upgradeType) {
	// @TODO
}

UnitOrUpgrade::~UnitOrUpgrade() { }

void UnitOrUpgrade::operator= (const BWAPI::UpgradeType &unitType) {
	// @TODO
}

bool UnitOrUpgrade::operator== (const BWAPI::UnitType &unitType) const {
	// @TODO
}

bool UnitOrUpgrade::operator== (const BWAPI::UpgradeType &upgradeType) const {
	// @TODO
}

bool UnitOrUpgrade::operator!= (const BWAPI::UpgradeType &unitType) const {
	// @TODO
}

BWAPI::UnitType UnitOrUpgrade::getUnitType() {
	return unitType;
}

BWAPI::UpgradeType UnitOrUpgrade::getUpgradeType() {
	return upgradeType;
}

bool UnitOrUpgrade::isUnitType() const {
	if (unitType != NULL) {
		return true;
	}
	return false;
}

bool UnitOrUpgrade::isUpgradeType() const {
	if (upgradeType != NULL) {
		return true;
	}
	return false;
}

int UnitOrUpgrade::gasPrice() {
	if (isUnitType()) {
		return getUnitType().gasPrice();
	}
	else {
		return getUpgradeType().gasPrice();
	}
}

void ResourceSpender::addUnitInvestment(BWAPI::UnitType unitType, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = UnitOrUpgrade(unitType);
	if (urgent) {
		investments.insert(investments.begin(), unitOrUpgrade);
	}
	else {
		investments.push_back(unitOrUpgrade);
	}
}

void ResourceSpender::addUpgradeInvestment(BWAPI::UpgradeType upgradeType, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = upgradeType;
	if (urgent) {
		investments.insert(investments.begin(), unitOrUpgrade);
	}
	else {
		investments.push_back(unitOrUpgrade);
	}
}

void ResourceSpender::addUnitInvestment(BWAPI::UnitType unitType, int position) {
	UnitOrUpgrade unitOrUpgrade = UnitOrUpgrade(unitType);
	investments.insert(investments.begin() + position, unitOrUpgrade);
}

void ResourceSpender::addUpgradeInvestment(BWAPI::UpgradeType upgradeType, int position) {
	UnitOrUpgrade unitOrUpgrade = upgradeType;
	investments.insert(investments.begin() + position, unitOrUpgrade);
}

void ResourceSpender::addRequirements(int number) {
	if (investments.size() >= 1) {

		// Add required buildings and upgrades
		for (int i = 0; i < investments[number].getUnitType().requiredUnits().size(); i++) {
			if (buildingUnitsPtr->getBuildingCount(investments[number].getUnitType().requiredUnits().at(i)) < 1 &&
				!unitInvestmentExists(investments[number].getUnitType().requiredUnits().at(i))) {
				addUnitInvestment(investments[number].getUnitType().requiredUnits().at(i), number);
				addRequirements(number);
			}
		}

		// Add Assimilators
		if (!investmentExists(investments[BWAPI::UnitTypes::Protoss_Assimilator])
			&&
			((investments[number].gasPrice() > 0 && !allIn) ||
			(investments[number].gasPrice() > Broodwar->self()->gas()))
			&&
			(unitsInProgress(BWAPI::UnitTypes::Protoss_Assimilator) +
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Assimilator) <
			(unitsInProgress(BWAPI::UnitTypes::Protoss_Nexus) +
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Nexus)))) {
			
			addUnitInvestment(BWAPI::UnitTypes::Protoss_Assimilator, number);
			addRequirements(number);
		}
	}
}

bool ResourceSpender::investmentExists(UnitOrUpgrade investment) {
	return false;
	// @TODO:
	//return (std::find(investments.begin(), investments.end(), investment) != investments.end());
}

bool ResourceSpender::unitInvestmentExists(UnitType investment) {
	return false;
	// @TODO:
	//return (std::find(investments.begin(), investments.end(), investment) != investments.end());
}

bool ResourceSpender::upgradeInvestmentExists(UpgradeType investment) {
	return false;
	//return (std::find(investments.begin(), investments.end(), investment) != investments.end());
}

bool ResourceSpender::workerNeeded() {
	return !allIn
		&&
		!defend
		&&
		!investmentExists(BWAPI::UnitTypes::Protoss_Probe)
		&&
		unitsInProgress(BWAPI::UnitTypes::Protoss_Probe) < buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Nexus)
		&&
		probeUnitsPtr->getWorkerCount() < MAX_WORKERS
		&&
		probeUnitsPtr->getWorkerCount() + MAX_WORKER_SURPLUS <
		(buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Nexus)) * (WORKERS_PER_MINERAL_LINE) +
		(buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Assimilator)) * (WORKERS_PER_GEYSER);
}

bool ResourceSpender::supplyNeeded() {
	return ((Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2 < (Broodwar->self()->supplyTotal() / 2) + getMaxSupplyOutput());
}

void ResourceSpender::setAllIn(bool status) {
	allIn = status;
}

void ResourceSpender::setDefend(bool status) {
	defend = status;
}

int ResourceSpender::unitsInProgress(BWAPI::UnitType unitType) {
	return unitHandlerPtr->getWarpingUnitCount(unitType);
}

bool ResourceSpender::upgradeInProgress(BWAPI::UpgradeType upgradeType) {
	return std::find(upgradesInProgress.begin(), upgradesInProgress.end(), upgradeType) != upgradesInProgress.end();
}

int ResourceSpender::getMaxSupplyOutput() {
	// Returns the maximum amount of unit supply that can be produced by all
	// buildings in the time that it takes to produce a supply provider

	int maxSupplyOutput = 0;

	maxSupplyOutput += BWAPI::UnitTypes::Protoss_Probe.supplyRequired() *
		buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Nexus) *
		(ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Probe.buildTime()));

	maxSupplyOutput += BWAPI::UnitTypes::Protoss_Probe.supplyRequired() *
		buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Gateway) *
		(ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Zealot.buildTime()));
	
	if (buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Robotics_Support_Bay) >= 1) {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Reaver.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) *
			(ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Reaver.buildTime()));
	}
	else {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Shuttle.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) *
			(ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Shuttle.buildTime()));
	}
	
	if (buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Fleet_Beacon) >= 1) {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Carrier.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Stargate) *
			(ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Carrier.buildTime()));
	}
	else if (buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Arbiter_Tribunal) >= 1) {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Arbiter.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Stargate) *
			(ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Arbiter.buildTime()));
	}
	else {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Scout.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Stargate) *
			(ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Scout.buildTime()));
	}
	
	return maxSupplyOutput;
}
