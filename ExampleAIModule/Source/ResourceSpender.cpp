#pragma once
#include "ResourceSpender.h"
#include "BuildingUnits.h"

using namespace BWAPI;

ResourceSpender::ResourceSpender() { }

ResourceSpender::~ResourceSpender() { }

bool ResourceSpender::_init(UnitHandler* unitHandler, BuildingUnits* buildingUnits) {
	if (buildingUnits == NULL) {
		return false;
	}

	unitHandlerPtr = unitHandler;
	buildingUnitsPtr = buildingUnits;

	return true;
}

UnitOrUpgrade::UnitOrUpgrade(BWAPI::UnitType unitType) { }

UnitOrUpgrade::UnitOrUpgrade(BWAPI::UpgradeType upgradeType) { }

UnitOrUpgrade::~UnitOrUpgrade() { }

BWAPI::UnitType getUnitType() {
	// @TODO
}

BWAPI::UpgradeType getUpgradeType() {
	// @TODO
}

bool UnitOrUpgrade::isUnitType() const {
	// @TODO
}

bool UnitOrUpgrade::isUpgradeType() const {
	// @TODO
}

int UnitOrUpgrade::gasPrice() {
	return 1;
	
	// @TODO
	/*if (isUnitType()) {
		return getUnitType().gasPrice();
	}
	else {
		return getUpgradeType().gasPrice();
	}*/
}

void ResourceSpender::addInvestment(BWAPI::UnitType unitType, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = UnitOrUpgrade(unitType);

	if (urgent) {
		investments.insert(investments.begin(), unitOrUpgrade);
	}
	else {
		investments.push_back(unitOrUpgrade);
	}
}

void ResourceSpender::addInvestment(BWAPI::UpgradeType upgradeType, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = UnitOrUpgrade(upgradeType);

	if (urgent) {
		investments.insert(investments.begin(), unitOrUpgrade);
	}
	else {
		investments.push_back(unitOrUpgrade);
	}
}

void ResourceSpender::addNeededTech(int priority) {
	if (investments.size() >= 1) {

		// @TODO: add needed buildings

		// More gas needed
		if (!investmentExists(investments[BWAPI::UnitTypes::Protoss_Assimilator])
			&&
			((investments[priority].gasPrice() > 0) || 
			(investments[priority].gasPrice() > Broodwar->self()->gas() && allIn))
			&&
			(unitsInProgress(BWAPI::UnitTypes::Protoss_Assimilator) +
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Assimilator) <
			(unitsInProgress(BWAPI::UnitTypes::Protoss_Nexus) +
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Nexus)))) {
			
			investments.insert(investments.begin(), BWAPI::UnitTypes::Protoss_Assimilator);
			addNeededTech(priority);
		}
	}
}

bool ResourceSpender::investmentExists(UnitOrUpgrade investment) {
	return true;

	// @TODO
	//return (std::find(investments.begin(), investments.end(), investment) != investments.end());
}

bool ResourceSpender::workerNeeded() {
	return !allIn &&
		!investmentExists(BWAPI::UnitTypes::Protoss_Probe) &&
		unitsInProgress(BWAPI::UnitTypes::Protoss_Probe) < buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Nexus);
}

bool ResourceSpender::supplyNeeded() {
	return ((Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2 < (Broodwar->self()->supplyTotal() / 2) + getMaxSupplyOutput());
}

void ResourceSpender::setAllIn(bool status) {
	allIn = status;
}

int ResourceSpender::unitsInProgress(BWAPI::UnitType unitType) {
	return unitHandlerPtr->getWarpingUnitCount(unitType);
}

bool ResourceSpender::upgradeInProgress(BWAPI::UpgradeType upgradeType) {
	return std::find(upgradesInProgress.begin(), upgradesInProgress.end(), upgradeType) != upgradesInProgress.end();
}

int ResourceSpender::getMaxSupplyOutput() {
	int maxSupplyOutput = 0;

	maxSupplyOutput += BWAPI::UnitTypes::Protoss_Probe.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Nexus) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Probe.buildTime()));
	maxSupplyOutput += BWAPI::UnitTypes::Protoss_Probe.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Gateway) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Zealot.buildTime()));
	
	if (buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Robotics_Support_Bay) >= 1) {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Reaver.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Reaver.buildTime()));
	}
	else {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Shuttle.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Shuttle.buildTime()));
	}
	
	if (buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Fleet_Beacon) >= 1) {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Carrier.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Stargate) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Carrier.buildTime()));
	}
	else if (buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Arbiter_Tribunal) >= 1) {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Arbiter.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Stargate) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Arbiter.buildTime()));
	}
	else {
		maxSupplyOutput += BWAPI::UnitTypes::Protoss_Scout.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Stargate) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Scout.buildTime()));
	}
	
	return maxSupplyOutput;
}
