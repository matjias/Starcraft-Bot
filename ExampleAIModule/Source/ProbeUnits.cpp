#pragma once
#include "ProbeUnits.h"

using namespace BWAPI;

ProbeUnits::ProbeUnits() {
	builder = NULL;
	previousBuildingToBuild = NULL;
}

ProbeUnits::~ProbeUnits() { }

void ProbeUnits::_init() {
}

void ProbeUnits::setBroodwarMock(Game* broodwarPtr) {
	BroodwarPtr = broodwarPtr;
}

void ProbeUnits::update(){
	/*if (mapAnalyzed){
		Position pos = Position(getOptimalBuildPlacement(UnitTypes::Protoss_Pylon, Broodwar->self()->getStartLocation()));
		Position bottomRight = Position(pos.x + UnitTypes::Protoss_Pylon.tileWidth() * TILE_SIZE,
										pos.y +  UnitTypes::Protoss_Pylon.tileHeight() * TILE_SIZE);
		
		Broodwar->drawBoxMap(pos, bottomRight, Colors::Green);

		pos = Position(Broodwar->getBuildLocation(UnitTypes::Protoss_Pylon, Broodwar->self()->getStartLocation()));
		bottomRight = Position(pos.x + UnitTypes::Protoss_Pylon.tileWidth() * TILE_SIZE,
			pos.y + UnitTypes::Protoss_Pylon.tileHeight() * TILE_SIZE);

		Broodwar->drawBoxMap(pos, bottomRight, Colors::Blue);
	}*/
	//mineMinerals(miningProbes);
	//mineGas(gasProbes);
}

void ProbeUnits::addUnit(Unit u){
	Unit field;
	Unit nexus = u->getClosestUnit(Filter::GetType == UnitTypes::Protoss_Nexus);
	int nexusId = 0;
	if (u->exists()){
		nexusId = nexus->getID();
	}
	if (workerCount < 1){
		miningProbes.insert(std::make_pair(nexusId, Unitset()));
		field = u->getClosestUnit(Filter::IsMineralField);
	}
	else{
		field = u->getClosestUnit(Filter::IsMineralField);
		field = Broodwar->getClosestUnit(Position(field->getPosition().x, 
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
	Unit tempProbe = NULL;
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
	if (builder == u){
		builder = NULL;
		return true;
	}
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
	for (Unitset::iterator it = setFrom->begin(); counter < amount; it++, counter++) {
		Unit probe = *it;
		setTo->insert(probe);
		setFrom->erase(probe); // @TODO: Fix crash
	}
}

// Build logic
//

bool ProbeUnits::newBuilding(BWAPI::UnitType building, TilePosition basePos){
	/*Position pos = Position(getOptimalBuildPlacement(building, basePos));
	Position bottomRight = Position(pos.x + building.tileWidth() * TILE_SIZE,
		pos.y + building.tileHeight() * TILE_SIZE);

	Broodwar->drawBoxMap(pos, bottomRight, Colors::Green);*/
	buildLocationCounter++;
	if (building == NULL) {
		if (builder != NULL) {
			addUnit(builder);
			builder = NULL;
		}
	}
	else {
		if (builder == NULL) {
			builder = extractUnit();
		}

		if (builder != NULL && (previousBuildingToBuild != building || buildLocationCounter >= BUILD_LOCATION_UPDATE_TIME)) {
			buildLocationCounter = 0;

			if (building == UnitTypes::Protoss_Assimilator) {
				previousBuildingToBuild = building;
				return builder->build(building, Broodwar->getBuildLocation(building, basePos));
			}
			else {
				previousBuildingToBuild = building;
				return builder->build(building, getOptimalBuildPlacement(building, basePos));
			}
		}
	}

	// Draw/print
	if (builder != NULL) {
		Broodwar->drawTextScreen(480, 100, "Builder ID: %i", builder->getID());
	}
	else {
		Broodwar->drawTextScreen(480, 100, "Builder ID: NULL");
	}
	
	return true;
}

// @TODO: It should ignore the builder when looking for a build loc
TilePosition ProbeUnits::getOptimalBuildPlacement(UnitType type, TilePosition basePos){
	TilePosition curPos = Broodwar->getBuildLocation(type, basePos);
	/*while (!checkMargin(type, curPos)){
		curPos = broodwar->getBuildLocation(type, basePos);
	}*/
	//return curPos;

	if (checkMargin(type, curPos)){
		return curPos;
	}

	// Maybe replace this with calling getBuildLocation again until a proper placement is found
	return recPlacement(type, curPos, 0);
}

TilePosition ProbeUnits::recPlacement(UnitType type, TilePosition basePos, int depth) {
	//Broodwar->sendText("custom Recursive placement in use");
	TilePosition curPos = basePos;
	for (int i = -depth; i <= depth; i++) {
		if (i == -depth || i == depth) {
			for (int j = -depth; j <= depth; j++) {
				curPos = TilePosition(basePos.x + i, basePos.y + j);
				if (checkMargin(type, curPos)) {
					return curPos;
				}
			}
		}
		curPos = TilePosition(basePos.x + i, basePos.y - depth);
		if (checkMargin(type, curPos)) {
			return curPos;
		}
		curPos = TilePosition(basePos.x + i, basePos.y + depth);
		if (checkMargin(type, curPos)) {
			return curPos;
		}
	}

	if (depth >= BUILD_LOCATION_FACTOR) {
		Broodwar->sendText("JUpti dub");
		return Broodwar->getBuildLocation(type, basePos);
	}
	return recPlacement(type, basePos, depth + 1);
}

bool ProbeUnits::checkMargin(UnitType type, TilePosition tilePos){
	//bool isBuildable = basePos.x <= 150 && basePos.y <= 150;
	//if (isBuildable){
		/*for (int i = -(ceil(type.dimensionLeft()/32.0)) - 1; i <= ceil(type.dimensionRight()/32.0) + 1; i++){
			for (int j = -(ceil(type.dimensionUp()/32.0)) - 1; j <= ceil(type.dimensionDown()/32.0) + 1; j++){
				if (!Broodwar->isBuildable(TilePosition(basePos.x + i, basePos.y + j), type) || unitBlocking(TilePosition(basePos.x + i, basePos.y + j))){
					return false;
				}
			}
		}*/

	bool unitsBlocking = tileAvailableFromUnits(type, tilePos);

	if (!unitsBlocking) {
		return false;
	}
	if (tilePos.x < 0 || tilePos.y < 0 || tilePos.x >= Broodwar->mapWidth() || tilePos.y >= Broodwar->mapWidth()) {
		return false;
	}
	if (type == UnitTypes::Protoss_Pylon){
		if(Broodwar->getUnitsInRadius(Position(tilePos), UnitTypes::Protoss_Pylon.sightRange(), Filter::GetType == type).size() > 0){
			return false;
		}
	}
	if (type.requiresPsi()){
		if (!Broodwar->hasPower(tilePos, type)){
			return false;
		}
	}

	bool canWalkAroundBuilding = true;

	for (int i = -1; i <= type.tileWidth(); i++) {
		for (int j = -1; j <= type.tileHeight(); j++) {
			TilePosition tileToCheck = TilePosition(tilePos.x + i, tilePos.y + j);
			if (!Broodwar->isBuildable(tileToCheck) ||
				!tileAvailableFromBuildings(tileToCheck)) {

				canWalkAroundBuilding = false;
			}
		}
	}

	/*return tileAvailableFromBuildings(TilePosition(basePos.x - 1, basePos.y - 1)) &&
		tileAvailableFromBuildings(TilePosition(basePos.x - 1, basePos.y + 1)) &&
		tileAvailableFromBuildings(TilePosition(basePos.x + 1, basePos.y - 1)) &&
		tileAvailableFromBuildings(TilePosition(basePos.x + 1, basePos.y + 1));*/


	//}
	// Fucker lucker det her ma friend
	return canWalkAroundBuilding;
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
void ProbeUnits::mineGas(Unit base, Unit geyser) {
	Unitset newSet;
	for (auto& uPair : miningProbes){
		moveUnits(&uPair.second, &newSet, WORKERS_PER_GEYSER);
	}
	newSet.gather(geyser);
	gasProbes.insert(std::make_pair(base->getID(), newSet));
}

void ProbeUnits::setAnalyzed(){
	mapAnalyzed = true;
}

bool ProbeUnits::tileAvailableFromUnits(UnitType type, TilePosition tilePos){
	// getUnitsinRectalngle instead mby
	/*return (Broodwar->getClosestUnit(Position(basePos))->getPosition().x <= Position(basePos).x + 32 &&
			Broodwar->getClosestUnit(Position(basePos))->getPosition().x >= Position(basePos).x - 32 &&
			Broodwar->getClosestUnit(Position(basePos))->getPosition().y <= Position(basePos).y + 32 &&
			Broodwar->getClosestUnit(Position(basePos))->getPosition().y >= Position(basePos).y - 32
		);*/

	Position posTopLeft = Position(tilePos);
	Position posBottomRight = Position(posTopLeft.x + type.tileWidth() * TILE_SIZE, 
										posTopLeft.y + type.tileHeight() * TILE_SIZE);

	Unitset unitsInRectangle = Broodwar->getUnitsInRectangle(posTopLeft, posBottomRight);

	if (unitsInRectangle.size() == 0) {
		return true;
	}
	else if (unitsInRectangle.size() > 1) {
		return false;
	}
	else {
		Unit u = *unitsInRectangle.begin();
		return u == builder;
	}

}

bool ProbeUnits::tileAvailableFromBuildings(TilePosition tilePos) {
	Position posTopLeft = Position(tilePos);
	Position posBottomRight = Position(posTopLeft.x + TILE_SIZE, posTopLeft.y + TILE_SIZE);
	Unitset unitsInRectangle = Broodwar->getUnitsInRectangle(posTopLeft, posBottomRight);

	if (unitsInRectangle.size() == 0) {
		return true;
	}
	else  {
		bool buildingOnTilePos = false;
		for (Unitset::iterator it = unitsInRectangle.begin();
			it != unitsInRectangle.end();
			it++) {

			Unit u = *it;
			if (u->getType().isBuilding()) {
				buildingOnTilePos = true;
			}
		}

		return !buildingOnTilePos;
	}
}