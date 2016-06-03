#pragma once
#include "ResourceSpender.h"
#include "BuildingUnits.h"

using namespace BWAPI;

ResourceSpender::ResourceSpender() { }

ResourceSpender::~ResourceSpender() { }

bool ResourceSpender::_init(BuildingUnits* buildingUnits) {
	if (buildingUnits == NULL) {
		return false;
	}

	buildingUnitsPtr = buildingUnits;

	return true;
}

UnitOrUpgrade::UnitOrUpgrade(BWAPI::UnitType unitType) { }

UnitOrUpgrade::UnitOrUpgrade(BWAPI::UpgradeType upgradeType) { }

UnitOrUpgrade::~UnitOrUpgrade() { }

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

}

bool ResourceSpender::workerNeeded() {

}

bool ResourceSpender::supplyNeeded() {
	return ((Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2 < (Broodwar->self()->supplyTotal() / 2) + getMaxSupplyOutput());
}

int ResourceSpender::getMaxSupplyOutput() {
	int totalSupplyOutput;

	totalSupplyOutput += BWAPI::UnitTypes::Protoss_Probe.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Nexus) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Probe.buildTime()));
	totalSupplyOutput += BWAPI::UnitTypes::Protoss_Probe.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Gateway) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Zealot.buildTime()));
	
	if (buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Robotics_Support_Bay) >= 1) {
		totalSupplyOutput += BWAPI::UnitTypes::Protoss_Reaver.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Reaver.buildTime()));
	}
	else {
		totalSupplyOutput += BWAPI::UnitTypes::Protoss_Shuttle.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Shuttle.buildTime()));
	}
	
	if (buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Fleet_Beacon) >= 1) {
		totalSupplyOutput += BWAPI::UnitTypes::Protoss_Carrier.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Stargate) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Carrier.buildTime()));
	}
	else if (buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Arbiter_Tribunal) >= 1) {
		totalSupplyOutput += BWAPI::UnitTypes::Protoss_Arbiter.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Stargate) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Arbiter.buildTime()));
	}
	else {
		totalSupplyOutput += BWAPI::UnitTypes::Protoss_Scout.supplyRequired() * buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Stargate) * (ceil(BWAPI::UnitTypes::Protoss_Pylon.buildTime() / BWAPI::UnitTypes::Protoss_Scout.buildTime()));
	}
	
	return totalSupplyOutput;
}
