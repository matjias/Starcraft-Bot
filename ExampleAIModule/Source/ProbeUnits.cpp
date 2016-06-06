#pragma once
#include "ProbeUnits.h"

ProbeUnits::ProbeUnits() { 
	miningCount = 1000;
}

ProbeUnits::~ProbeUnits() { }

using namespace BWAPI;

void ProbeUnits::mineMinerals(Unitset uSet) {
	uSet.gather(uSet.getClosestUnit(Filter::IsMineralField));
}

void ProbeUnits::mineMinerals(Unit u) {
	u->gather(u->getClosestUnit(Filter::IsMineralField));
}

void ProbeUnits::mineGas(Unitset uSet) {
	uSet.gather(uSet.getClosestUnit(Filter::GetType == UnitTypes::Protoss_Assimilator));
}

void ProbeUnits::addUnit(Unit u){
	mineMinerals(u);
	miningProbes.insert(u);
}

Unitset* ProbeUnits::getMiningUnits(){
	return &miningProbes;
}

Unit ProbeUnits::extractUnit(){
	Unit tempProbe;
	int i = 0;
	for (auto& probe : miningProbes){
		Broodwar->sendText("Skete der noget: %i", i);
		if (!probe->isCarryingMinerals()){
			Broodwar->sendText("Fandt en lile lort");
			tempProbe = probe;
			break;
		}
		tempProbe = probe;
		i++;
	}
	miningProbes.erase(tempProbe);
	return tempProbe;
}