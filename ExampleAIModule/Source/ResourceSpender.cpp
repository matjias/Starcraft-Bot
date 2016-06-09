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

int UnitOrUpgrade::mineralPrice() {
	if (isUnitType()) {
		return getUnitType().mineralPrice();
	}
	else {
		return getUpgradeType().mineralPrice();
	}
}

int UnitOrUpgrade::gasPrice() {
	if (isUnitType()) {
		return getUnitType().gasPrice();
	}
	else {
		return getUpgradeType().gasPrice();
	}
}

void ResourceSpender::update() {
	if (workerNeeded()) {
		addUnitInvestment(BWAPI::UnitTypes::Protoss_Probe, true);
	}
	if (supplyNeeded()) {
		addUnitInvestment(BWAPI::UnitTypes::Protoss_Pylon, true);
	}
	addRequirements(0);

	calculateReservedResources();
	purchase();

	// Draw/print investment list
	for (int i = 0; i < investments.size(); i++) {
		if (investments[i].isUnitType()) {
			Broodwar->drawTextScreen(5, 5 + i * 10, "%i: %s", i, investments[i].getUnitType().c_str());
		}
		else {
			Broodwar->drawTextScreen(5, 5 + i * 10, "%i: %s", i, investments[i].getUpgradeType().c_str());
		}
	}
}

void ResourceSpender::purchase() {
	for (int i = 0; i < investments.size(); i++) {
		if (!investments[i].pending) {
			if ((investments[i].isUnitType() && canBuildUnit(investments[i].getUnitType())) ||
				(investments[i].isUpgradeType() && canUpgrade(investments[i].getUpgradeType()))) {

				investments[i].pending = true;

				if (investments[i].isUnitType()) {
					unitHandlerPtr->purchase(investments[i].getUnitType());
				}
				else {
					unitHandlerPtr->purchase(investments[i].getUpgradeType());
				}
			}
			break;
		}
	}
}

void ResourceSpender::calculateReservedResources() {
	reservedMinerals = 0;
	reservedGas = 0;

	for (int i = 0; i < investments.size(); i++) {
		if (investments[i].pending) {
			reservedMinerals += investments[i].mineralPrice();
			reservedGas += investments[i].gasPrice();
		}
	}
}

void ResourceSpender::addUnitInvestment(BWAPI::UnitType investment, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = UnitOrUpgrade(investment);

	if (!investmentExists(investment)) {
		if (urgent) {
			investments.insert(investments.begin(), unitOrUpgrade);
		}
		else {
			investments.push_back(unitOrUpgrade);
		}
	}
}

void ResourceSpender::addUpgradeInvestment(BWAPI::UpgradeType investment, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = investment;

	if (!investmentExists(investment)) {
		if (urgent) {
			investments.insert(investments.begin(), unitOrUpgrade);
		}
		else {
			investments.push_back(unitOrUpgrade);
		}
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

bool ResourceSpender::canBuildUnit(BWAPI::UnitType unitType) {
	
	// Is the unit/building affordable?
	if (unitType.mineralPrice() - reservedMinerals > Broodwar->self()->minerals() ||
		unitType.gasPrice() - reservedGas > Broodwar->self()->gas() ||
		unitType.supplyRequired() > (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2) {

		return false;
	}

	// Are the required buildings owned?
	auto requiredUnits = unitType.requiredUnits();

	for (auto& unitType : requiredUnits){
		if (buildingUnitsPtr->getBuildingCount(unitType.first) < 1 &&
			!investmentExists(unitType.first)) {

			return false;
		}
	}

	// Are any of the buildings producing the unit idle?
	if (!unitType.isBuilding() &&
		buildingUnitsPtr->getIdleBuilding(unitType.whatBuilds().first) == NULL) {

		return false;
	}

	return true;
}

bool ResourceSpender::canUpgrade(BWAPI::UpgradeType upgradeType) {
	
	// Is the upgrade affordable?
	if (upgradeType.mineralPrice() - reservedMinerals > Broodwar->self()->minerals() ||
		upgradeType.gasPrice() - reservedGas > Broodwar->self()->gas()) {

		return false;
	}

	// Are the required buildings owned?
	if (buildingUnitsPtr->getBuildingCount(upgradeType.whatUpgrades()) < 1 &&
		buildingUnitsPtr->getBuildingCount(upgradeType.whatsRequired()) < 1) {

		return false;
	}

	// Are any of the buildings producing the upgrade idle?
	if (buildingUnitsPtr->getIdleBuilding(upgradeType.whatUpgrades()) == NULL) {

		return false;
	}

	return true;
}

void ResourceSpender::addRequirements(int number) {
	if (investments.size() >= 1) {

		// Add required buildings and upgrades
		auto requiredUnits = investments[number].getUnitType().requiredUnits();

		for (auto& unitType : requiredUnits){
			if (buildingUnitsPtr->getBuildingCount(unitType.first) < 1 &&
				!investmentExists(unitType.first)) {
				
				addUnitInvestment(unitType.first, number);
				addRequirements(number);
			}
		}

		// Add Assimilators
		if (!investmentExists(BWAPI::UnitTypes::Protoss_Assimilator)
			&&
			((investments[number].gasPrice() > 0 && !allIn) ||
			(investments[number].gasPrice() > Broodwar->self()->gas()))
			&&
			(unitsInProgress(BWAPI::UnitTypes::Protoss_Assimilator) +
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Assimilator) <
			(unitsInProgress(BWAPI::UnitTypes::Protoss_Nexus) +
			buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Nexus)))) {
			
			addUnitInvestment(BWAPI::UnitTypes::Protoss_Assimilator, number);
		}
	}
}

bool ResourceSpender::investmentExists(UnitType unitType) {
	for (int i = 0; i < investments.size(); i++) {
		if (investments[i].getUnitType() == unitType) {
			return true;
		}
	}

	return false;
}

bool ResourceSpender::investmentExists(UpgradeType upgradeType) {
	for (int i = 0; i < investments.size(); i++) {
		if (investments[i].getUpgradeType() == upgradeType) {
			return true;
		}
	}

	return false;
}

bool ResourceSpender::workerNeeded() {
	return true;

	// @TODO
	/*return !allIn
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
		(buildingUnitsPtr->getBuildingCount(BWAPI::UnitTypes::Protoss_Assimilator)) * (WORKERS_PER_GEYSER);*/
}

bool ResourceSpender::supplyNeeded() {
	
	return Broodwar->self()->supplyUsed() / 2 + 
		2 > // @TODO: getMaxSupplyOutput() > 
		Broodwar->self()->supplyTotal() / 2 + 
		unitHandlerPtr->getWarpingUnitCount(BWAPI::UnitTypes::Protoss_Pylon) * BWAPI::UnitTypes::Protoss_Pylon.supplyProvided() +
		unitHandlerPtr->getWarpingUnitCount(BWAPI::UnitTypes::Protoss_Nexus) * BWAPI::UnitTypes::Protoss_Pylon.supplyProvided();
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
