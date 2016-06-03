#pragma once
#include "ResourceSpender.h"

using namespace BWAPI;

ResourceSpender::ResourceSpender() { }

ResourceSpender::~ResourceSpender() { }

bool ResourceSpender::_init(UnitHandler* unitHandler) {
	if (unitHandler == NULL) {
		return false;
	}

	unitHandlerPtr = unitHandler;

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
	return ((Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2 < getMaxSupplyOutput() + (Broodwar->self()->supplyTotal() / 2));
}

int ResourceSpender::getMaxSupplyOutput() {
	return BWAPI::UnitTypes::Protoss_Probe.supplyRequired() +
	BWAPI::UnitTypes::Protoss_Zealot.supplyRequired() +
	BWAPI::UnitTypes::Protoss_Reaver.supplyRequired() +
	BWAPI::UnitTypes::Protoss_Carrier.supplyRequired();
}
