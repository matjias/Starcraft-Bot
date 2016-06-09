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
		TilePosition pos = getOptimalBuildPlacement(UnitTypes::Protoss_Pylon, Broodwar->self()->getStartLocation());
		Broodwar->drawCircleMap(Position(pos), 32, Colors::Green);
		Broodwar->sendText("Dis is da pos sis tions: %i, %i", pos.x, pos.y);
		TilePosition pos1 = Broodwar->getBuildLocation(UnitTypes::Protoss_Pylon, Broodwar->self()->getStartLocation());
		Broodwar->sendText("std::Loc  %i, %i", pos1.x, pos1.y);
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


// Build logic
//
bool ProbeUnits::newBuilding(UnitType type, TilePosition basePos){
	Unit u = Broodwar->getClosestUnit(Position(basePos), Filter::GetType == UnitTypes::Protoss_Probe);
	u->build(type, getOptimalBuildPlacement(type, basePos));
	return true;
}

TilePosition ProbeUnits::getOptimalBuildPlacement(UnitType type, TilePosition basePos){
	TilePosition curPos = Broodwar->getBuildLocation(type, basePos);
	int radius = 1;
	if (checkMargin(type, curPos)){
		return curPos;
	}
	// Maybe replace this with calling getBuildLocation again until a proper placement is found
	return recPlacement(type, basePos, radius);
}

TilePosition ProbeUnits::recPlacement(UnitType type, TilePosition basePos, int depth){
	TilePosition curPos = basePos;
	for (int i = -depth; i <= depth; i++){
		if (i == -depth || i == depth){
			for (int j = -depth; j <= depth; j++){
				curPos = TilePosition(basePos.x + i, basePos.y + j);
				if (checkMargin(type, curPos)){
					return curPos;
				}
			}
		}
		curPos = TilePosition(basePos.x + i, basePos.y);
		if (checkMargin(type, curPos)){
			return curPos;
		}
		curPos = TilePosition(basePos.x, basePos.y + i);
		if (checkMargin(type, curPos)){
			return curPos;
		}
	}
	return recPlacement(type, basePos, depth + 1);
}

bool ProbeUnits::checkMargin(UnitType type, TilePosition basePos){
	for (int i = -(type.dimensionLeft()/32) - 1; i <= type.dimensionRight()/32 + 1; i++){
		for (int j = -(type.dimensionUp()/32) - 1; j <= type.dimensionDown()/32 + 1; j++){
			if (!Broodwar->isBuildable(TilePosition(basePos.x + i, basePos.y + j), type)){
				return false;
			}
		}
	}
	// Fucker lucker det her ma friend
	return basePos.x <= 150 && basePos.y <= 150;
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