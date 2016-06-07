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
	//mineMinerals(u);
	miningProbes.insert(u);
	//getOptimalBuildPlacement(UnitTypes::Protoss_Gateway, Broodwar->self()->getStartLocation());
}

Unit ProbeUnits::extractUnit(){
	Unit tempProbe;
	for (auto& probe : miningProbes){
		if (!probe->isCarryingMinerals()){
			tempProbe = probe;
			break;
		}
		tempProbe = probe;
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
	bool ableToBuild = false;
	TilePosition curPos = basePos;
	while (!ableToBuild){
		ableToBuild = checkMargin(type, curPos);
		if (!ableToBuild){
			int radius = BWTA::getRegion(curPos)->getPolygon().getNearestPoint(Position(curPos)).getDistance(Point<int,1>(curPos.x,curPos.y));
			curPos = TilePosition(curPos.x + (std::rand() % (radius + radius + 1) - radius), curPos.y + (std::rand() % (radius + radius + 1) - radius));
			Broodwar->sendText("le pos dos buildz: %i, %i", curPos.x, curPos.y);
		}
	}
	return curPos;
}

bool ProbeUnits::checkMargin(UnitType type, TilePosition basePos){
	for (int i = -1; i <= 1; i++){
		for (int j = -1; j <= 1; j++){
			if (!Broodwar->canBuildHere(TilePosition(basePos.x + i, basePos.y + j), type)){
				return false;
			}

		}
	}
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

void ProbeUnits::decreaseGasMiners(int amount){
	moveUnits(&gasProbes, &miningProbes, amount);
	update();
}

void ProbeUnits::mineGas(Unitset uSet) {
	// Insert logic for distribution of probes
	uSet.gather(uSet.getClosestUnit(Filter::GetType == UnitTypes::Protoss_Assimilator), true);
}