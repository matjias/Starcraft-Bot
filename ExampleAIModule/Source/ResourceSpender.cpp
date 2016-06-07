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

UnitOrUpgrade::UnitOrUpgrade(BWAPI::UnitType investment) {
	unitType = investment;
}

UnitOrUpgrade::UnitOrUpgrade(BWAPI::UpgradeType investment) {
	upgradeType = investment;
}

UnitOrUpgrade::~UnitOrUpgrade() { }

bool UnitOrUpgrade::operator== (const UnitOrUpgrade &investment) const {
	if (unitType != NULL && investment.isUnitType()) {
		if (unitType == investment.getUnitType()) {
			return true;
		}
	}
	if (upgradeType != NULL && investment.isUpgradeType()) {
		if (unitType == investment.getUnitType()) {
			return true;
		}
	}
	return false;
}

bool UnitOrUpgrade::operator!= (const UnitOrUpgrade &investment) const {
	return UnitOrUpgrade::operator== (investment);
}

BWAPI::UnitType UnitOrUpgrade::getUnitType() const {
	return unitType;
}

BWAPI::UpgradeType UnitOrUpgrade::getUpgradeType() const {
	return upgradeType;
}

bool UnitOrUpgrade::isUnitType() const {
	return unitType != NULL;
}

bool UnitOrUpgrade::isUpgradeType() const {
	return upgradeType != NULL;
}

int UnitOrUpgrade::gasPrice() {
	if (isUnitType()) {
		return getUnitType().gasPrice();
	}
	else {
		return getUpgradeType().gasPrice();
	}
}


void ResourceSpender::addUnitInvestment(BWAPI::UnitType investment, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = UnitOrUpgrade(investment);
	if (urgent) {
		investments.insert(investments.begin(), unitOrUpgrade);
	}
	else {
		investments.push_back(unitOrUpgrade);
	}
}

void ResourceSpender::addUpgradeInvestment(BWAPI::UpgradeType investment, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = investment;
	if (urgent) {
		investments.insert(investments.begin(), unitOrUpgrade);
	}
	else {
		investments.push_back(unitOrUpgrade);
	}
}

void ResourceSpender::addUnitInvestment(BWAPI::UnitType investment, int position) {
	UnitOrUpgrade unitOrUpgrade = UnitOrUpgrade(investment);
	investments.insert(investments.begin() + position, unitOrUpgrade);
}

void ResourceSpender::addUpgradeInvestment(BWAPI::UpgradeType investment, int position) {
	UnitOrUpgrade unitOrUpgrade = investment;
	investments.insert(investments.begin() + position, unitOrUpgrade);
}

void ResourceSpender::addRequirements(int number) {
	if (investments.size() >= 1) {

		// Add required buildings and upgrades
		/*for (int i = 0; i < investments[number].getUnitType().requiredUnits().size(); i++) {
			if (buildingUnitsPtr->getBuildingCount(investments[number].getUnitType().requiredUnits().at(i)) < 1 &&
				!investmentExists(investments[number].getUnitType().requiredUnits().at(i))) {
				addUnitInvestment(investments[number].getUnitType().requiredUnits().at(i), number);
				addRequirements(number);
			}
		}*/

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
	return std::find(investments.begin(), investments.end(), investment) != investments.end();
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
