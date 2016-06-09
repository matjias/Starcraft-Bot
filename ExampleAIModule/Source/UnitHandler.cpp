#pragma once
#include "UnitHandler.h"

using namespace BWAPI;


UnitHandler::UnitHandler() {
	scoutUnits.setUnitHandler(this);
}

UnitHandler::~UnitHandler() { }

void UnitHandler::_init(){
	combatUnits._init();
}

void UnitHandler::addWarpingUnit(Unit u){
	warpingUnits.insert(std::pair<int, Unit>(u->getID(), u));
}

void UnitHandler::removeWarpingUnit(Unit u){
	warpingUnits.erase(u->getID());
}

// Deciding where the discovered unit belongs
void UnitHandler::addUnit(Unit u){
	removeWarpingUnit(u);
	if (isCombatUnit(u)){
		combatUnits.addUnit(u);
		units.insert(std::pair<int, UnitPlacement>(u->getID(), combat));
	}
	else if (isProbeUnit(u)){
		probeUnits.addUnit(u);
		units.insert(std::pair<int, UnitPlacement>(u->getID(), probe));
	}
	else if (isBuilding(u)){
		buildingUnits.addBuilding(u);
		units.insert(std::pair<int, UnitPlacement>(u->getID(), building));
	}
}

void UnitHandler::addScout(UnitType unitType) {
	if (unitType == UnitTypes::Protoss_Probe) {
		Unit u = probeUnits.extractUnit();
		units[u->getID()] = scout;
		scoutUnits.addUnit(u);
	}
}

// Expand on tactical decisions. What is our current strategy 
// and should the unit in this situation really be added here
// 
bool UnitHandler::isCombatUnit(Unit u){
	//Add all unit types that needs to be handle by the army
	return u->getType() == UnitTypes::Protoss_Zealot || u->getType() == UnitTypes::Protoss_Dragoon;
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
	scoutUnits.updateScouts();
	probeUnits.update();
}

void UnitHandler::setAnalyzed(){
	probeUnits.setAnalyzed();
}

bool UnitHandler::purchase(UnitType unitType) {
	
	// Purchase units
	if (unitType.isBuilding()) {
		if (buildingUnits.getIdleBuilding(unitType.whatBuilds().first) != NULL) {
			return buildingUnits.getIdleBuilding(unitType.whatBuilds().first)->train(unitType);
		}
	}
	
	// Purchase buildings
	else {
		// @TODO
	}

	return false;
}

bool UnitHandler::purchase(UpgradeType upgradeType) {
	// @TODO
	return false;
}

void UnitHandler::deleteUnit(Unit u){
	
}
