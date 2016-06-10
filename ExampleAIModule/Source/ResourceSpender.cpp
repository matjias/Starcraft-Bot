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

UnitOrUpgrade::UnitOrUpgrade(UnitType investment) {
	unitType = investment;
}

UnitOrUpgrade::UnitOrUpgrade(UpgradeType investment) {
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

UnitType UnitOrUpgrade::getUnitType() const {
	return unitType;
}

UpgradeType UnitOrUpgrade::getUpgradeType() const {
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
		addUnitInvestment(UnitTypes::Protoss_Probe, true);
	}
	if (supplyNeeded()) {
		addUnitInvestment(UnitTypes::Protoss_Pylon, true);
	}

	addAllRequirements();
	clearReservedResources();
	setPendingInvestments();
	purchase();

	// Draw/print
	Broodwar->drawTextScreen(280, 5, "Reserved minerals: %i", reservedMinerals);
	Broodwar->drawTextScreen(280, 15, "Reserved gas: %i", reservedGas);
	Broodwar->drawTextScreen(280, 25, "Nexuses: %i", buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Nexus));
	Broodwar->drawTextScreen(280, 35, "Supply output: %i", getMaxSupplyOutput());

	for (int i = 0; i < investments.size(); i++) {
		if (investments[i].isUnitType()) {
			if (investments[i].pending) {
				Broodwar->drawTextScreen(280, 60 + i * 10, "%i: %s (Pending)", i, investments[i].getUnitType().c_str());
			}
			else {
				Broodwar->drawTextScreen(280, 60 + i * 10, "%i: %s", i, investments[i].getUnitType().c_str());
			}
		}
		else {
			if (investments[i].pending) {
				Broodwar->drawTextScreen(280, 60 + i * 10, "%i: %s (Pending)", i, investments[i].getUpgradeType().c_str());
			}
			else {
				Broodwar->drawTextScreen(280, 50 + i * 10, "%i: %s", i, investments[i].getUpgradeType().c_str());
			}
		}
	}
}

void ResourceSpender::setPendingInvestments() {

	bool falseReached = false;

	for (int i = 0; i < investments.size(); i++) {

		if (!falseReached &&
			((investments[i].isUnitType() && investments[i].getUnitType().isBuilding() && canBuildBuilding(investments[i].getUnitType())) ||
			(investments[i].isUnitType() && !investments[i].getUnitType().isBuilding() && canBuildUnit(investments[i].getUnitType())) ||
			(investments[i].isUpgradeType() && canUpgrade(investments[i].getUpgradeType())))) {

			investments[i].pending = true;
			calculateReservedResources();
		}
		else {
			investments[i].pending = false;
			falseReached = true;
		}
	}
}

void ResourceSpender::purchase() {

	UnitType buildingToBuild = NULL;

	for (int i = 0; i < investments.size(); i++) {
		if (investments[i].pending) {
			if (investments[i].isUnitType()) {
				if (investments[i].getUnitType().isBuilding()) {
					if (buildingToBuild == NULL) {
						buildingToBuild = investments[i].getUnitType();
					}
				}
				else if (unitHandlerPtr->purchaseUnit(investments[i].getUnitType())) {
					removeInvestment(i);
				}
			}
			else if (unitHandlerPtr->purchaseUpgrade(investments[i].getUpgradeType())) {
				removeInvestment(i);
			}
		}
	}

	unitHandlerPtr->purchaseBuilding(buildingToBuild);
	Broodwar->drawTextScreen(280, 45, "%s", buildingToBuild.c_str());
}

void ResourceSpender::clearReservedResources() {
	reservedMinerals = 0;
	reservedGas = 0;
}

void ResourceSpender::calculateReservedResources() {
	clearReservedResources();

	for (int i = 0; i < investments.size(); i++) {
		if (investments[i].pending) {
			reservedMinerals += investments[i].mineralPrice();
			reservedGas += investments[i].gasPrice();
		}
	}
}

void ResourceSpender::addUnitInvestment(UnitType investment, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = UnitOrUpgrade(investment);
	removeInvestments(investment);
	if (urgent) {
		investments.insert(investments.begin(), unitOrUpgrade);
	}
	else {
		investments.push_back(unitOrUpgrade);
	}
	
}

void ResourceSpender::addUpgradeInvestment(UpgradeType investment, bool urgent) {
	UnitOrUpgrade unitOrUpgrade = investment;
	removeInvestments(investment);
	if (urgent ) {
		investments.insert(investments.begin(), unitOrUpgrade);
	}
	else {
		investments.push_back(unitOrUpgrade);
	}
}

void ResourceSpender::addUnitInvestment(UnitType investment, int position) {
	UnitOrUpgrade unitOrUpgrade = UnitOrUpgrade(investment);
	investments.insert(investments.begin() + position, unitOrUpgrade);
}

void ResourceSpender::addUpgradeInvestment(UpgradeType investment, int position) {
	UnitOrUpgrade unitOrUpgrade = investment;
	investments.insert(investments.begin() + position, unitOrUpgrade);
}

void ResourceSpender::removeInvestment(int position) {
	investments.erase(investments.begin() + position);
}

bool ResourceSpender::removeInvestments(UnitType investment) {
	bool itemRemoved = false;

	for (int i = 0; i < investments.size(); i++) {
		if (investments[i].isUnitType() && investments[i].getUnitType() == investment) {
			investments.erase(investments.begin() + i);
			itemRemoved = true;
		}
	}
	return itemRemoved;
}

bool ResourceSpender::removeInvestments(UpgradeType investment) {
	bool itemRemoved = false;

	for (int i = 0; i < investments.size(); i++) {
		if (investments[i].isUpgradeType() && investments[i].getUpgradeType() == investment) {
			investments.erase(investments.begin() + i);
			itemRemoved = true;
		}
	}
	return itemRemoved;
}

void ResourceSpender::removeAllDublicates() {
	for (int i = 0; i < investments.size(); i++) {
		removeDublicates(i);
	}
}

bool ResourceSpender::removeDublicates(int number) {
	bool itemRemoved = false;

	for (int i = number + 1; i < investments.size(); i++) {
		if (investments[i] == investments[number]) {
			investments.erase(investments.begin() + i);
			itemRemoved = true;
		}
	}
	return itemRemoved;
}

bool ResourceSpender::canBuildUnit(UnitType unitType) {
	
	// Is the unit affordable?
	if (unitType.mineralPrice() > Broodwar->self()->minerals() - reservedMinerals ||
		unitType.gasPrice() > Broodwar->self()->gas() - reservedGas ||
		unitType.supplyRequired() / 2 > (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2) {

		return false;
	}

	// Are the required buildings owned?
	auto requiredUnits = unitType.requiredUnits();

	for (auto& unitType : requiredUnits) {
		if (buildingUnitsPtr->getBuildingCount(unitType.first) == 0 &&
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

bool ResourceSpender::canBuildBuilding(UnitType buildingType) {

	// Is the building affordable?
	if (buildingType.mineralPrice() > Broodwar->self()->minerals() - reservedMinerals ||
		buildingType.gasPrice() > Broodwar->self()->gas() - reservedGas) {

		return false;
	}

	// Are the required buildings owned?
	auto requiredUnits = buildingType.requiredUnits();

	for (auto& unitType : requiredUnits) {
		if (unitType.first.isBuilding() &&
			buildingUnitsPtr->getBuildingCount(unitType.first) == 0 &&
			!investmentExists(unitType.first)) {

			return false;
		}
	}

	// Are there any workers
	if (probeUnitsPtr->getWorkerCount() == 0) {

		return false;
	}

	return true;
}

bool ResourceSpender::canUpgrade(UpgradeType upgradeType) {
	
	// Is the upgrade affordable?
	if (upgradeType.mineralPrice() > Broodwar->self()->minerals() - reservedMinerals ||
		upgradeType.gasPrice() > Broodwar->self()->gas() - reservedGas) {

		return false;
	}

	// Are the required buildings owned?
	if (buildingUnitsPtr->getBuildingCount(upgradeType.whatUpgrades()) == 0 &&
		buildingUnitsPtr->getBuildingCount(upgradeType.whatsRequired()) == 0) {

		return false;
	}

	// Are any of the buildings producing the upgrade idle?
	if (buildingUnitsPtr->getIdleBuilding(upgradeType.whatUpgrades()) == NULL) {

		return false;
	}

	return true;
}

void ResourceSpender::addAllRequirements() {
	if (investments.size() >= 1) {
		for (int i = 0; i < investments.size(); i++) {
			addRequirements(i);
		}
	}
}

void ResourceSpender::addRequirements(int number) {
	
	// Add required buildings
	auto requiredUnits = investments[number].getUnitType().requiredUnits();

	for (auto& unitType : requiredUnits){
		if (unitType.first.isBuilding() &&
			buildingUnitsPtr->getBuildingCount(unitType.first) < 1) {

			addUnitInvestment(unitType.first, number);
		}
	}

	// Add Pylon
	if (investments[number].isUnitType() &&
		investments[number].getUnitType().requiresPsi() &&
		buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Pylon) +
		unitHandlerPtr->getWarpingUnitCount(UnitTypes::Protoss_Pylon) < 1) {

		addUnitInvestment(UnitTypes::Protoss_Pylon, number);
	}

	// @TODO: gasPrice is always 0?
	// Add Assimilator
	if (((investments[number].gasPrice() > 0 && !allIn && !defend) ||
		(investments[number].gasPrice() > Broodwar->self()->gas()))
		&&
		(unitsInProgress(UnitTypes::Protoss_Assimilator) +
		buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Assimilator) <
		(unitsInProgress(UnitTypes::Protoss_Nexus) +
		buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Nexus)))) {
		
		addUnitInvestment(UnitTypes::Protoss_Assimilator, number);
	}

	// Remove repeating elements in investment list
	removeAllDublicates();
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
	return !allIn
		&&
		!defend
		&&
		!investmentExists(UnitTypes::Protoss_Probe)
		&&
		unitsInProgress(UnitTypes::Protoss_Probe) < buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Nexus)
		&&
		probeUnitsPtr->getWorkerCount() < MAX_WORKERS
		&&
		probeUnitsPtr->getWorkerCount() <
		(buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Nexus)) * (WORKERS_PER_MINERAL_LINE) +
		(buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Assimilator)) * (WORKERS_PER_GEYSER) + 
		MAX_WORKER_SURPLUS;
}

bool ResourceSpender::supplyNeeded() {
	return Broodwar->self()->supplyUsed() / 2 +
		getMaxSupplyOutput() >=
		(Broodwar->self()->supplyTotal() + 
		unitHandlerPtr->getWarpingUnitCount(UnitTypes::Protoss_Pylon) * UnitTypes::Protoss_Pylon.supplyProvided() +
		unitHandlerPtr->getWarpingUnitCount(UnitTypes::Protoss_Nexus) * UnitTypes::Protoss_Nexus.supplyProvided()) / 2;
}

void ResourceSpender::removePlacedBuildings(UnitType buildingType) {
	removeInvestments(buildingType);
}

void ResourceSpender::setAllIn(bool status) {
	allIn = status;
}

void ResourceSpender::setDefend(bool status) {
	defend = status;
}

int ResourceSpender::unitsInProgress(UnitType unitType) {
	return unitHandlerPtr->getWarpingUnitCount(unitType);
}

bool ResourceSpender::upgradeInProgress(UpgradeType upgradeType) {
	return std::find(upgradesInProgress.begin(), upgradesInProgress.end(), upgradeType) != upgradesInProgress.end();
}

int ResourceSpender::getMaxSupplyOutput() {
	// Returns the maximum amount of unit supply that can be produced by all
	// buildings in the time that it takes to produce a supply provider

	int maxSupplyOutput = 0;

	maxSupplyOutput += UnitTypes::Protoss_Probe.supplyRequired() *
		buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Nexus) *
		(ceil((float)UnitTypes::Protoss_Pylon.buildTime() / (float)UnitTypes::Protoss_Probe.buildTime()));

	maxSupplyOutput += UnitTypes::Protoss_Zealot.supplyRequired() *
		buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Gateway) *
		(ceil((float)UnitTypes::Protoss_Pylon.buildTime() / (float)UnitTypes::Protoss_Zealot.buildTime()));
	
	if (buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Robotics_Support_Bay) >= 1) {
		maxSupplyOutput += UnitTypes::Protoss_Reaver.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Robotics_Facility) *
			(ceil((float)UnitTypes::Protoss_Pylon.buildTime() / (float)UnitTypes::Protoss_Reaver.buildTime()));
	}
	else {
		maxSupplyOutput += UnitTypes::Protoss_Shuttle.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Robotics_Facility) *
			(ceil((float)UnitTypes::Protoss_Pylon.buildTime() / (float)UnitTypes::Protoss_Shuttle.buildTime()));
	}
	
	if (buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Fleet_Beacon) >= 1) {
		maxSupplyOutput += UnitTypes::Protoss_Carrier.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Stargate) *
			(ceil((float)UnitTypes::Protoss_Pylon.buildTime() / (float)UnitTypes::Protoss_Carrier.buildTime()));
	}
	else if (buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Arbiter_Tribunal) >= 1) {
		maxSupplyOutput += UnitTypes::Protoss_Arbiter.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Stargate) *
			(ceil((float)UnitTypes::Protoss_Pylon.buildTime() / (float)UnitTypes::Protoss_Arbiter.buildTime()));
	}
	else {
		maxSupplyOutput += UnitTypes::Protoss_Scout.supplyRequired() *
			buildingUnitsPtr->getBuildingCount(UnitTypes::Protoss_Stargate) *
			(ceil((float)UnitTypes::Protoss_Pylon.buildTime() / (float)UnitTypes::Protoss_Scout.buildTime()));
	}
	
	return maxSupplyOutput / 2;
}
