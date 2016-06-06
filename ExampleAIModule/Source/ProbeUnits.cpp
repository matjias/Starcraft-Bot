#pragma once
#include "ProbeUnits.h"

ProbeUnits::ProbeUnits() { 
	miningCount = 1000;
}

ProbeUnits::~ProbeUnits() { }

using namespace BWAPI;

void ProbeUnits::update(){
	mineMinerals(miningProbes);
	mineGas(gasProbes);
}

void ProbeUnits::addUnit(Unit u){
	mineMinerals(u);
	miningProbes.insert(u);
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

void ProbeUnits::moveUnits(Unitset *setFrom, Unitset *setTo, int amount){
	int counter = 0;
	for (Unitset::iterator i = setFrom->begin(); counter < amount; i++) {
		Unit probe = *i;
		setTo->insert(probe);
		setFrom->erase(probe);
	}
}

bool ProbeUnits::newBuilding(UnitType type){
	//fack
	return true;
}

bool ProbeUnits::newBuilding(UnitType type, TilePosition pos){
	Unit u = Broodwar->getClosestUnit(Position(pos), Filter::GetType == UnitTypes::Protoss_Zealot);
	u->build(type, pos);
	return true;
}

// Mining Units
//
void ProbeUnits::mineMinerals(Unitset uSet) {
	uSet.gather(uSet.getClosestUnit(Filter::IsMineralField));
}

void ProbeUnits::mineMinerals(Unit u) {
	u->gather(u->getClosestUnit(Filter::IsMineralField));
}

Unitset* ProbeUnits::getMiningUnits(){
	return &miningProbes;
}


// Gas Units
//
void ProbeUnits::increaseGasMiners(int amount){
	moveUnits(&miningProbes, &gasProbes, amount);
	update();
}

int ProbeUnits::getWorkerCount() {
	return miningProbes.size();
}

void ProbeUnits::decreaseGasMiners(int amount){
	moveUnits(&gasProbes, &miningProbes, amount);
	update();
}

void ProbeUnits::mineGas(Unitset uSet) {
	// Insert logic for distribution of probes
	uSet.gather(uSet.getClosestUnit(Filter::GetType == UnitTypes::Protoss_Assimilator), true);
}