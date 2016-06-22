#pragma once
#include "UnitHandler.h"

using namespace BWAPI;


UnitHandler::UnitHandler() {
	scoutUnits.setUnitHandler(this);
}

UnitHandler::~UnitHandler() { }

void UnitHandler::_init(){
}

void UnitHandler::setBroodwarMock(Game* mockBroodwarPtr) {
	BroodwarPtr = mockBroodwarPtr;
	probeUnits.setBroodwarMock(mockBroodwarPtr);
}

void UnitHandler::addWarpingUnit(Unit u){
	if (units.count(u->getID()) == 0){
		warpingUnits.insert(std::make_pair(u->getType(), u));
	}
}

void UnitHandler::removeWarpingUnit(Unit u){
	warpingUnits.erase(u->getType());
	// @TODO: Should do something like this. For the moment the above works.
	/*std::pair<std::multimap<UnitType, Unit>::iterator, std::multimap<UnitType, Unit>::iterator> itr = warpingUnits.equal_range(u->getType());
	std::multimap<UnitType, Unit>::iterator it = itr.first;
	for (; it != itr.second; ++it) {
		if (it->second == u) {
			Broodwar->sendText("fucker locker med remove warp");
			warpingUnits.erase(it);
			break;
		}
	}*/
}

// Deciding where the discovered unit belongs
void UnitHandler::addUnit(Unit u) {
	//Broodwar->sendText("Das type ge tilfuyt %s", u->getType().c_str());
	
	if (units.count(u->getID()) != 0){
		units.erase(u->getID());
	}
	else{
		// If not in unit map, it must be a new unit?
		removeWarpingUnit(u);
	}

	if (isCombatUnit(u)){
		units.insert(std::pair<int, UnitPlacement>(u->getID(), combat));
		combatUnits.addUnit(u);
		armyValue += u->getType().mineralPrice() + u->getType().gasPrice()*GAS_TO_MINERALS;
	}
	else if (isProbeUnit(u)){
		units.insert(std::pair<int, UnitPlacement>(u->getID(), probe));
		probeUnits.addUnit(u);
	}
	else if (isBuilding(u)){
		units.insert(std::pair<int, UnitPlacement>(u->getID(), building));
		buildingUnits.addBuilding(u);
	}
	if (u->getType() == UnitTypes::Protoss_Assimilator){

		probeUnits.mineGas(u->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Nexus), u);
	}
	
}

bool UnitHandler::addScout(UnitType unitType) {
	if (unitType == UnitTypes::Protoss_Probe) {
		Unit u = probeUnits.extractUnit();
		if (u == NULL){
			return false;
		}
		units[u->getID()] = scout;
		return scoutUnits.addUnit(u);
	}
	return false;
}

// Expand on tactical decisions. What is our current strategy 
// and should the unit in this situation really be added here
// 
bool UnitHandler::isCombatUnit(Unit u){
	//Add all unit types that needs to be handle by the army
	return u->getType() == UnitTypes::Protoss_Zealot ||
		u->getType() == UnitTypes::Protoss_Dragoon ||
		u->getType() == UnitTypes::Protoss_Archon ||
		u->getType() == UnitTypes::Protoss_Dark_Templar ||
		u->getType() == UnitTypes::Protoss_Observer;
}
// #unnecessarybooleanfunction lols
bool UnitHandler::isProbeUnit(Unit u){
	//Add probes
	return u->getType() == UnitTypes::Protoss_Probe;
}

bool UnitHandler::isBuilding(Unit u){
	return u->getType().isBuilding();
}

BuildingUnits* UnitHandler::getBuildingUnits() {
	return &buildingUnits;
}

CombatUnits* UnitHandler::getCombatUnits() {
	return &combatUnits;
}

ProbeUnits* UnitHandler::getProbeUnits() {
	return &probeUnits;
}

ScoutUnits* UnitHandler::getScoutUnits() {
	return &scoutUnits;
}

int UnitHandler::getWarpingUnitCount(BWAPI::UnitType unitType) {
	return warpingUnits.count(unitType);
}

void UnitHandler::update() {
	probeUnits.update();
	scoutUnits.updateScouts();
	combatUnits.update();
}

void UnitHandler::setAnalyzed(){
	probeUnits.setAnalyzed();
	scoutUnits.setAnalyzed();
}

bool UnitHandler::deleteUnit(Unit u){
	
	UnitPlacement enMum = units[u->getID()];
	bool isDeleted = false;
	switch (enMum){
	case probe:
		isDeleted = probeUnits.deleteUnit(u);
		break;
	case combat:
		armyValue -= u->getType().mineralPrice() + u->getType().gasPrice()*GAS_TO_MINERALS;
		isDeleted = combatUnits.deleteUnit(u);
		break;
	case scout:
		isDeleted = scoutUnits.deleteUnit(u);
		break;
	case building:
		isDeleted = buildingUnits.deleteUnit(u);
		break;
	}
	return isDeleted;
}

bool UnitHandler::purchaseUnit(UnitType unitType) {
	Unit builderUnit = buildingUnits.getIdleBuilding(unitType.whatBuilds().first);
	if (builderUnit != NULL) {
		//Broodwar->sendText("%s bygger %s", builderUnit->getType(), unitType);
		return builderUnit->train(unitType);
	}
	return false;
}

bool UnitHandler::purchaseBuilding(BWAPI::UnitType building) {
	return probeUnits.newBuilding(building, Broodwar->self()->getStartLocation());
}

bool UnitHandler::purchaseUpgrade(UpgradeType upgradeType) {
	Unit builderUnit = buildingUnits.getIdleBuilding(upgradeType.whatUpgrades());
	if (builderUnit != NULL) {
		return builderUnit->upgrade(upgradeType);
	}
	return false;
}

float UnitHandler::getArmyValue() {
	// @TODO: Get army value from combat units, 1 gas = 1 GAS_TO_MINERALS
	return armyValue;
}

void UnitHandler::setArmyBalance(float value) {
	armyBalance = value;
}
