#pragma once
#include "ProbeUnits.h"

ProbeUnits::ProbeUnits() { 
}

ProbeUnits::~ProbeUnits() { }

using namespace BWAPI;

void ProbeUnits::update(){
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
	Unit field;
	int nexusId = u->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Nexus)->getID();
	if (workerCount < 1){
		miningProbes.insert(std::pair<int, Unitset>(nexusId, Unitset()));
		field = u->getClosestUnit(Filter::IsMineralField && !Filter::IsBeingGathered);
}
	else{
		field = u->getClosestUnit(Filter::IsMineralField);
		field = Broodwar->getClosestUnit(Position(field->getPosition().x, field->getPosition().y - 32/*Magisk tal*/ * (workerCount % 3)), Filter::IsMineralField && !Filter::IsBeingGathered);
		if (field == NULL){
			field = u->getClosestUnit(Filter::IsMineralField);
		}
	}
	miningProbes[nexusId].insert(u);
	u->gather(field, true);
	workerCount++;
}

Unit ProbeUnits::extractUnit(){
	Unit tempProbe;
	Unitset *uSet = &miningProbes.begin()->second;
	for (auto& probe : *uSet){
		if (!probe->isCarryingMinerals()){
			Broodwar->sendText("Fandt en lile lort");
			tempProbe = probe;
			break;
		}
		tempProbe = probe;
	}
	uSet->erase(tempProbe);
	return tempProbe;
}

void ProbeUnits::deleteUnit(Unit u){
	for (auto& probePair : miningProbes){
		if (probePair.second.contains(u)){
			probePair.second.erase(u);
		}
	}
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

void ProbeUnits::mineNewBase(Unit base) {
	Unitset newSet = Unitset();
	for (auto& uPair : miningProbes){
		moveUnits(&uPair.second, &newSet, WORKERS_PER_MINERAL_LINE / miningProbes.size());
}
	newSet.gather(base->getClosestUnit(Filter::IsMineralField));
	miningProbes.insert(std::pair<int, Unitset>(base->getID(), newSet));
}

Unitset* ProbeUnits::getMiningUnits(){
	return &miningProbes.begin()->second;
}
int ProbeUnits::getWorkerCount() {
	return workerCount;
}

// Gas Units
//

void ProbeUnits::mineGas(Unit base) {
	Unitset newSet = Unitset();
	for (auto& uPair : miningProbes){
		moveUnits(&uPair.second, &newSet, WORKERS_PER_GEYSER);
}
	newSet.gather(base->getClosestUnit(Filter::IsMineralField));
	miningProbes.insert(std::pair<int, Unitset>(base->getID(), newSet));
}

void ProbeUnits::setAnalyzed(){
	mapAnalyzed = true;
}