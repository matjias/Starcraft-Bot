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

Unitset* ProbeUnits::getMiningUntis(){
	return &miningProbes;
}