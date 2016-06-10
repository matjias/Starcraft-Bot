#pragma once
#include "ProbeUnits.h"

using namespace BWAPI;

ProbeUnits::ProbeUnits() { 
	builder = NULL;
}

ProbeUnits::~ProbeUnits() { }

void ProbeUnits::_init(Game* broodwarPtr) {
	broodwar = broodwarPtr;
}



void ProbeUnits::update(){
	if (mapAnalyzed){
		Position pos = Position(getOptimalBuildPlacement(UnitTypes::Protoss_Pylon, broodwar->self()->getStartLocation()));
		broodwar->drawBoxMap(Position(pos.x - 64, pos.y - 32), Position(pos.x + 64, pos.y + 64), Colors::Green);
	}
	//mineMinerals(miningProbes);
	//mineGas(gasProbes);
}

void ProbeUnits::addUnit(Unit u){
	Unit field;
	int nexusId = u->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Nexus)->getID();
	if (workerCount < 1){
		miningProbes.insert(std::make_pair(nexusId, Unitset()));
		field = u->getClosestUnit(Filter::IsMineralField);
	}
	else{
		field = u->getClosestUnit(Filter::IsMineralField);
		field = broodwar->getClosestUnit(Position(field->getPosition().x, 
			field->getPosition().y - TILE_SIZE * (workerCount % 3)), 
			Filter::IsMineralField && !Filter::IsBeingGathered);

		if (field == NULL){
			field = u->getClosestUnit(Filter::IsMineralField);
		}
	}
	miningProbes[nexusId].insert(u);
	u->gather(field);
	workerCount++;
	}

Unit ProbeUnits::extractUnit(){
	Unit tempProbe;
	Unitset *uSet = &miningProbes.begin()->second;
	for (auto& probe : *uSet){
		if (!probe->isCarryingMinerals()){
			tempProbe = probe;
			break;
		}
		tempProbe = probe;
	}
	uSet->erase(tempProbe);
	workerCount--;
	return tempProbe;
}

bool ProbeUnits::deleteUnit(Unit u){
	for (auto& probePair : miningProbes){
		if (probePair.second.contains(u)){
			probePair.second.erase(u);
			workerCount--;
			return true;
		}
	}
	for (auto& probePair : gasProbes){
		if (probePair.second.contains(u)){
			probePair.second.erase(u);
			workerCount--;
			return true;
}
	}
	return false;
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
	// @TODO 6-10: Don't take the scout and the gas miners!?
	Unit builder = broodwar->getClosestUnit(Position(basePos), Filter::GetType == UnitTypes::Protoss_Probe);
	

	// @TODO 6-10: Make this work
	/*if (builder != u) {
		if (builder != NULL) {
			builder->stop(); // @TODO 6-10: Replace stop by: Mine minerals at assigned base
		}
		builder = u;
	}*/

	// @TODO 6-10: Make this work: Move to build location, build when u can, return true when building is placed
	/*if (u->getDistance(getOptimalBuildPlacement(type, basePos) < 32)) {
	u->build(type, getOptimalBuildPlacement(type, basePos));
		builder = NULL;
		builder->stop(); // @TODO 6-10: Replace stop by: Mine minerals at assigned base
		return true;
	}
	else {
		u->move(getOptimalBuildPlacement(type, basePos), false);
		return false;
	}*/

	// @TODO 6-10: Remove the following
	builder->build(type, getOptimalBuildPlacement(type, basePos));
	return true;
}

// @TODO 6-10: It should ignore the builder when looking for a build loc
TilePosition ProbeUnits::getOptimalBuildPlacement(UnitType type, TilePosition basePos){
	TilePosition curPos = Broodwar->getBuildLocation(type, basePos);
	//while (!checkMargin(type, curPos)){
	//	curPos = Broodwar->getBuildLocation(type, basePos);
	//}
	//return curPos;
	int radius = 1;
	if (checkMargin(type, curPos)){
		return curPos;
	}
	// Maybe replace this with calling getBuildLocation again until a proper placement is found
	return recPlacement(type, basePos, radius);
}

TilePosition ProbeUnits::recPlacement(UnitType type, TilePosition basePos, int depth){
	broodwar->sendText("custom Recursive placement in use");
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
		curPos = TilePosition(basePos.x, basePos.y - i);
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
	bool isBuildable = basePos.x <= 150 && basePos.y <= 150;
	if (isBuildable){
		for (int i = -(ceil(type.dimensionLeft()/32.0)) - 1; i <= ceil(type.dimensionRight()/32.0) + 1; i++){
			for (int j = -(ceil(type.dimensionUp()/32.0)) - 1; j <= ceil(type.dimensionDown()/32.0) + 1; j++){
				if (!broodwar->isBuildable(TilePosition(basePos.x + i, basePos.y + j), type) || unitBlocking(TilePosition(basePos.x + i, basePos.y + j))){
				return false;
			}
		}
	}
	}
	// Fucker lucker det her ma friend
	return isBuildable;
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

bool ProbeUnits::unitBlocking(TilePosition basePos){
	// getUnitsinRectalngle instead mby
	Unit tempUnit = broodwar->getClosestUnit(Position(basePos));
	if (builder == tempUnit){
		return false;
	}
	return (tempUnit->getPosition().x <= Position(basePos).x + 32 &&
			tempUnit->getPosition().x >= Position(basePos).x - 32 &&
			tempUnit->getPosition().y <= Position(basePos).y + 32 &&
			tempUnit->getPosition().y >= Position(basePos).y - 32
		);
}