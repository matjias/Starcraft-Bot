#pragma once
#include "ProbeUnits.h"

ProbeUnits::ProbeUnits() { 
	miningCount = 1000;
}

ProbeUnits::~ProbeUnits() { }

using namespace BWAPI;

void ProbeUnits::update(){
	Broodwar->sendText("bibibi");
	if (mapAnalyzed){
		TilePosition pos = getOptimalBuildPlacement(UnitTypes::Protoss_Gateway, TilePosition(miningProbes.getPosition()));
		Broodwar->drawCircleMap(Position(pos), 64, Colors::Green);
		Broodwar->sendText("Dis is da pos sis tions: %i, %i", pos.x, pos.y);
	}
	//mineMinerals(miningProbes);
	//mineGas(gasProbes);
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


bool ProbeUnits::newBuilding(UnitType type, TilePosition basePos){
	Unit u = Broodwar->getClosestUnit(Position(basePos), Filter::GetType == UnitTypes::Protoss_Probe);
	u->build(type, getOptimalBuildPlacement(type, basePos));
	return true;
}

TilePosition ProbeUnits::getOptimalBuildPlacement(UnitType type, TilePosition basePos){
	TilePosition curPos = Broodwar->getBuildLocation(type, basePos, 2);
	//while (!checkMargin(type, curPos)){
	//	curPos = Broodwar->getBuildLocation(type, basePos);
	//}
	


	return curPos;
}

bool ProbeUnits::checkMargin(UnitType type, TilePosition basePos){
	/*
	for (int i = -1; i <= 1; i++){
		for (int j = -1; j <= 1; j++){
			if (!Broodwar->canBuildHere(TilePosition(basePos.x + i, basePos.y + j), type)){
				return false;
			}

		}
	}*/
	return Broodwar->isBuildable(basePos, true);
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

void ProbeUnits::setAnalyzed(bool analyzed){
	mapAnalyzed = analyzed;
}