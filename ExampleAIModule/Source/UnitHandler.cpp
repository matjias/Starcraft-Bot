#pragma once
#include "UnitHandler.h"

using namespace BWAPI;


UnitHandler::UnitHandler() { }

UnitHandler::~UnitHandler() { }

void UnitHandler::_init(){
	combatUnits._init();
}

// Deciding where the discovered unit belongs
void UnitHandler::addUnit(Unit u){
	if (isCombatUnit(u)){
		combatUnits.addUnit(u);
	}
	else if (isProbeUnit(u)){
		// Add functionality for gas probes. 
		miningProbes.insert(u);
		probeUnits.mineMinerals(u);
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