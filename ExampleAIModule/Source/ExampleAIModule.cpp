#pragma once
#include "ExampleAIModule.h"
#include "Scouting.h"
#include "Army.h"
#include <iostream>
#include <vector>

using namespace BWAPI;
using namespace Filter;

const int MAX_SUPPLY = 200;
const int MAX_WORKERS = 100;
const int MINERAL_SURPLUS_LIMIT = 1000;
const int GAS_SURPLUS_LIMIT = 1000;

Unit builder;

int stepsWaited;

int availableSupply;
int supplyBuffer;
int reservedMinerals;
int reservedGas;

int nexusCount;
int nexusesWarping;
int nexusesQueued;
int pylonCount;
int pylonsWarping;
int pylonsQueued;
int gatewayCount;
int gatewaysWarping;
int gatewaysQueued;

int probeCount;
int probesQueued;
int probesWarping;
int zealotCount;
int zealotsWarping;
int zealotsQueued;

std::vector<UnitType>builderOrder;
std::vector<UnitType>investmentList;

bool buildOrderFinished;

std::vector<Unit>gateways;

Scouting scoutClass;

Army army;

void ExampleAIModule::onStart() {
	Broodwar->enableFlag(Flag::UserInput);
	Broodwar->setCommandOptimizationLevel(2);
	
	stepsWaited = 0;

	nexusCount = 1;
	probeCount = 4;
	builder = 0;

	buildOrderFinished = true;

	availableSupply = (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2;

	// Scouting stuff
	scoutClass._init(Broodwar->getStartLocations(), Broodwar->self()->getStartLocation());
}

void ExampleAIModule::onEnd(bool isWinner) {
	
}

void ExampleAIModule::onFrame() {
	availableSupply = (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2;

	//Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	//Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	//Broodwar->drawTextScreen(200, 20, "Available Supply: %d + %d", availableSupply, supplyBuffer);
	//Broodwar->drawTextScreen(200, 40, "Gateways: %d", gatewayCount);
	
	for (int i = 0; i < investmentList.size(); i++) {
		//Broodwar->drawTextScreen(200, 60 + i * 20, "%i: %s", i, investmentList.at(i).c_str());
	}

	//Broodwar->drawTextScreen(200, 40, "Reserved minerals: %d", reservedMinerals);
	Broodwar->drawTextScreen(350, 20, "Scout distance: %i", scoutClass.getDistance());
	Broodwar->drawTextScreen(350, 40, "Location: %i, %i", scoutClass.getX(), scoutClass.getY());

	Broodwar->drawTextScreen(350, 60, "Found Enemy: %d", scoutClass.returnFoundEnemyBase());
	Broodwar->drawTextScreen(350, 80, "Location: %i, %i", scoutClass.returnEnemyBaseLocs().x, scoutClass.returnEnemyBaseLocs().y);
	
	//Broodwar->drawTextScreen(350, 100, "Zeallala: %i", zealots.size());

	/*
	TilePosition::list spawns = scoutClass.getSpawns();
	for (int i = 0; i < spawns.size(); i++) {
		Broodwar->drawTextScreen(50, 20 * i + 10, "Spawn %i, {%d, %d} dist: %d, dist to scout: %d", 
			i + 1, Position(spawns.at(i)).x, Position(spawns.at(i)).y, 
			Position(spawns.at(i)).getApproxDistance(Position(spawns.at(spawns.size() - 1))), 
			scoutClass.getScout().getApproxDistance(Position(spawns.at(i))));
	}
	*/
	TilePosition::list spawns = scoutClass.getDynamicSpawns();
	std::vector<bool> spawnsBool = scoutClass.getDynamicSpawnBools();
	for (int i = 0; i < spawns.size(); i++) {
		bool toDraw = spawnsBool.at(i) ? 1 : 0;

		Broodwar->drawTextScreen(5, 20 * i + 20, "Spawn %i, {%d, %d} dist: %d, dist to scout: %d, scouted: %d",
			i + 1, Position(spawns.at(i)).x, Position(spawns.at(i)).y,
			Position(spawns.at(i)).getApproxDistance(Position(Broodwar->self()->getStartLocation())),
			scoutClass.getScout().getApproxDistance(Position(spawns.at(i))),
			toDraw);
	}
	
	
	//Broodwar->drawTextScreen(20, 0, "Reserved Min: %d", reservedMinerals);
	//Broodwar->drawTextScreen(20, 20, "Reserved Gas: %d", reservedGas);
	//Broodwar->drawTextScreen(20, 40, "Nexuses: %d, W: %d, Q: %d", nexusCount, nexusesWarping, nexusesQueued);
	//Broodwar->drawTextScreen(20, 60, "Pylons: %d, W: %d, Q: %d", pylonCount, pylonsWarping, pylonsQueued);
	//Broodwar->drawTextScreen(20, 80, "Gateways: %d, W: %d, Q: %d", gatewayCount, gatewaysWarping,  gatewaysQueued);
	//Broodwar->drawTextScreen(20, 100, "Workers: %d, W: %d, Q: %d", probeCount, probesWarping, probesQueued);

	// Calculate supply
	availableSupply = (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2;

	// AI Logic goes here
	if (scoutClass.isScouting()) {
		scoutClass.updateScout();
	}
	
	if (!buildOrderFinished) {

	}

	// Add resource invesments
	if (pylonNeeded()) {
		investmentList.push_back(UnitTypes::Protoss_Pylon);
		supplyBuffer += UnitTypes::Protoss_Pylon.supplyProvided() / 2;
		pylonsQueued++;
	}
	if (gatewayNeeded()) {
		investmentList.push_back(UnitTypes::Protoss_Gateway);
		gatewaysQueued++;
	}

	if (workerNeeded()) {
		investmentList.push_back(UnitTypes::Protoss_Probe);
		probesQueued++;
	}
	if (zealotNeeded()) {
		investmentList.push_back(UnitTypes::Protoss_Zealot);
		zealotsQueued++;
	}

	// Reorder investment priorities
	if (gatewaysQueued
		&& gatewayCount <= zealotsWarping
		&& investmentList[0] == UnitTypes::Protoss_Zealot) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == UnitTypes::Protoss_Gateway) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), UnitTypes::Protoss_Gateway);
				break;
			}
		}
	}
	if (zealotsQueued
		&& zealotsWarping < gatewayCount
		&& Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY
		&& investmentList[0] != UnitTypes::Protoss_Zealot) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == UnitTypes::Protoss_Zealot) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), UnitTypes::Protoss_Zealot);
				break;
			}
		}
	}
	if (probesQueued
		&& probesWarping < nexusCount
		&& Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY
		&& investmentList[0] != UnitTypes::Protoss_Probe) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == UnitTypes::Protoss_Probe) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), UnitTypes::Protoss_Probe);
				break;
			}
		}
	}
	if (pylonsQueued
		&& availableSupply <= nexusCount + gatewayCount * 2
		&& Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY
		&& investmentList[0] != UnitTypes::Protoss_Pylon) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == UnitTypes::Protoss_Pylon) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), UnitTypes::Protoss_Pylon);
				break;
			}
		}
	}

	
	// All Units loop
	for (auto &u : Broodwar->self()->getUnits()) {

		// If unit does not exist or is still being made, we ignore it
		if (!u->exists() || !u->isCompleted()) {
			continue;
		}
		
		// Worker logic
		if (u->getType().isWorker()) {

			// Assign scout
			//if (!scoutClass.isScouting() || !scoutClass.returnFoundEnemyBase()) {
			if (!scoutClass.hasAssignedScout() && !scoutClass.returnFoundEnemyBase()) {
				scoutClass.assignScout(u);
				if (u == builder) {
					builder = 0;
				}
			}

			// Assign builder
			if (builder == 0 && !scoutClass.isScout(u)) {
				builder = u;
			}
			
			// Assign build command
			if (u == builder
				&& !investmentList.empty()
				&& investmentList[0].isBuilding()
				&& Broodwar->self()->minerals() >= investmentList[0].mineralPrice() + reservedMinerals
				&& Broodwar->self()->gas() >= investmentList[0].gasPrice() + reservedGas) {
				buildBuilding(u, investmentList[0]);
			}

			// Mine minerals
			else if (!scoutClass.isScout(u)) {
				mineMinerals(u);
			}
			
			// Gotta mine dem bitcoins
			/*if (u->isIdle()) {
				if (u->isCarryingMinerals()) {
					u->returnCargo();
				} else {
					u->gather(u->getClosestUnit(IsMineralField));
				}
			}
		}
		
		// Resource Depot (central stuff or something)
		else if (u->getType().isResourceDepot()) {
			if (u->isIdle() && Broodwar->self()->minerals() >= 50 + reservedMinerals &&
				(availableSupply > 1 || Broodwar->self()->incompleteUnitCount(u->getType().getRace().getSupplyProvider()) > 0)) {
				u->train(u->getType().getRace().getWorker());
			}
			
			if (supplyNeeded(u)) {
				buildSupply(u);
			}

			if (canBuildWorker(u) && workerNeeded(u)) {
				buildWorker(u);*/
			
		}

		// Nexus logic
		else if (u->getType().isResourceDepot()
			&& u->isIdle()
			&& !investmentList.empty()
			&& investmentList[0] == UnitTypes::Protoss_Probe
			&& Broodwar->self()->minerals() >= investmentList[0].mineralPrice() + reservedMinerals
			&& Broodwar->self()->gas() >= investmentList[0].gasPrice() + reservedGas
			&& availableSupply >= investmentList[0].supplyRequired() / 2) {
			//(availableSupply > 1 || Broodwar->self()->incompleteUnitCount(u->getType().getRace().getSupplyProvider()) > 0)) {
			buildProbe(u);
		}

			// Build Gateways
			/*if (Broodwar->self()->minerals() >= 150 + reservedMinerals) {
				UnitType building = UnitTypes::Protoss_Gateway;

				static int lastCheck = 0;

				if (lastCheck + 400 < Broodwar->getFrameCount()) {

					lastCheck = Broodwar->getFrameCount();

					Unit builder = u->getClosestUnit(GetType == building.whatBuilds().first &&
						(IsIdle || IsGatheringMinerals) && IsOwned);

					if (builder) {
						TilePosition buildLocation = Broodwar->getBuildLocation(building, builder->getTilePosition());

						if (buildLocation) {

							// Box to display where the building is placed
							Broodwar->registerEvent([buildLocation, building](Game*) {
								Broodwar->drawBoxMap(Position(buildLocation),
									Position(buildLocation + building.tileSize()),
									Colors::Blue);
							}, nullptr,
								building.buildTime() + 100);

							// Build the building
							builder->build(building, buildLocation);*/

		// Gateway logic
		else if (u->getType() == UnitTypes::Protoss_Gateway
			&& u->isIdle()
			&& !investmentList.empty()
			&& investmentList[0] == UnitTypes::Protoss_Zealot
			&& Broodwar->self()->minerals() >= investmentList[0].mineralPrice() + reservedMinerals
			&& Broodwar->self()->gas() >= investmentList[0].gasPrice() + reservedGas
			&& availableSupply >= investmentList[0].supplyRequired() / 2) {
			//(availableSupply > 1 || Broodwar->self()->incompleteUnitCount(u->getType().getRace().getSupplyProvider()) > 0)) {
			if (army.buildZealot(u)){
				availableSupply -= investmentList[0].supplyRequired() / 2;
				zealotsQueued--;
				zealotsWarping++;
				investmentList.erase(investmentList.begin());
			}
		}
		
		
		/*else if (u->getType() == UnitTypes::Protoss_Gateway && Broodwar->self()->minerals() - reservedMinerals >= 100 && availableSupply >= 2 && !gateways.empty()){
			reservedMinerals += 100;
			buildZealot(u);
		}*/
		
		// Zealot attack logic
		army.run(scoutClass);
	}
}

void ExampleAIModule::onSendText(std::string text) {
	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());
}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text) {
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player) {
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target) {

}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit) {
	if (unit->getType().isBuilding() && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		reservedMinerals -= unit->getType().mineralPrice();
		reservedGas -= unit->getType().gasPrice();
	}

	if (unit->getType() == UnitTypes::Protoss_Pylon && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		pylonsQueued--;
		pylonsWarping++;
	}
	if (unit->getType() == UnitTypes::Protoss_Gateway && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		gatewaysQueued--;
		gatewaysWarping++;
	}

	/*if (unit->getType() == UnitTypes::Protoss_Pylon && unit->getPlayer() == Broodwar->self()) {
		reservedMinerals -= 100;
	}
	if (unit->getType() == UnitTypes::Protoss_Gateway && unit->getPlayer() == Broodwar->self()) {
		gateways.push_back(unit);
		reservedMinerals -= 150;
	}*/

	if (unit->getType() == UnitTypes::Protoss_Zealot && unit->getPlayer() == Broodwar->self()){
		army.addZealot(unit);
		//reservedMinerals -= 100;
	}

	if (unit->getPlayer() != Broodwar->self() && unit->getType().isResourceDepot()) {
		scoutClass.foundEnemyBase(TilePosition(unit->getPosition()));
	}
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit) {
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit) {
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit) {
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit) {
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit) {
	if (unit->getType() == UnitTypes::Protoss_Probe && unit->getPlayer() == Broodwar->self()) {
		probeCount--;
	}
	else if (unit->getType() == UnitTypes::Protoss_Zealot && unit->getPlayer() == Broodwar->self()) {
		zealotCount--;
	}

	if (scoutClass.isScout(unit)) {
		scoutClass.scoutHasDied();
	}
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit) {
	
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit) {
}

void ExampleAIModule::onSaveGame(std::string gameName) {
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit) {
	if (unit->getType() == UnitTypes::Protoss_Pylon && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		supplyBuffer -= unit->getType().supplyProvided() / 2;
		pylonsWarping--;
		pylonCount++;
	}
	if (unit->getType() == UnitTypes::Protoss_Gateway && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		gatewaysWarping--;
		gatewayCount++;
	}
	if (unit->getType() == UnitTypes::Protoss_Probe && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		probesWarping--;
		probeCount++;
	}
	if (unit->getType() == UnitTypes::Protoss_Zealot && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		zealotsWarping--;
		zealotCount++;
	}
}

void ExampleAIModule::buildBuilding(BWAPI::Unit builder, BWAPI::UnitType building) {

	TilePosition buildLocation = Broodwar->getBuildLocation(building, builder->getTilePosition());

	if (buildLocation) {

		// Box to display where the building is placed
		Broodwar->registerEvent([buildLocation, building](Game*) {
			Broodwar->drawBoxMap(Position(buildLocation),
				Position(buildLocation + building.tileSize()),
				Colors::Blue);
		}, nullptr,
			building.buildTime() + 100);

		// Making the building
		builder->build(building, buildLocation);

		reservedMinerals += building.mineralPrice();

		investmentList.erase(investmentList.begin());
	}
}

bool ExampleAIModule::pylonNeeded() {
	return (!pylonCount
		&& !pylonsWarping
		&& !pylonsQueued
		|| availableSupply + supplyBuffer <= (nexusCount + nexusesWarping) * 1 + (gatewayCount + gatewaysWarping) * 2
		&& Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY);
}

bool ExampleAIModule::canBuildSupply() {
	if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Pylon.mineralPrice() + reservedMinerals
		&& Broodwar->self()->gas() >= UnitTypes::Protoss_Pylon.gasPrice() + reservedGas) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == UnitTypes::Protoss_Pylon) {
				return false;
			}
		}
		return true;
	}
	return false;
}

/*void ExampleAIModule::buildSupply(BWAPI::Unit u) {
	buildingsNeeded.push_back(UnitTypes::Protoss_Pylon);
	
	UnitType pylon = u->getType().getRace().getSupplyProvider();
	static int lastCheck = 0;

	if (lastCheck + 400 < Broodwar->getFrameCount() &&
		Broodwar->self()->incompleteUnitCount(pylon) == 0) {

		lastCheck = Broodwar->getFrameCount();

		Unit supplyBuilder = u->getClosestUnit(GetType == pylon.whatBuilds().first &&
			(IsIdle || IsGatheringMinerals) && IsOwned);

		if (supplyBuilder) {
			TilePosition buildLocation = Broodwar->getBuildLocation(pylon, supplyBuilder->getTilePosition());

			if (buildLocation) {

				// Box to display where the building is placed
				Broodwar->registerEvent([buildLocation, pylon](Game*) {
					Broodwar->drawBoxMap(Position(buildLocation),
						Position(buildLocation + pylon.tileSize()),
						Colors::Blue);
				}, nullptr,
					pylon.buildTime() + 100);

				// Making the building
				supplyBuilder->build(pylon, buildLocation);

				pylonCount++;
				reservedMinerals += 100;
			}
		}
	}
}*/

bool ExampleAIModule::workerNeeded() {
	return !probesQueued && probeCount + probesQueued < MAX_WORKERS;
}

bool ExampleAIModule::canbuildProbe() {
	return availableSupply >= UnitTypes::Protoss_Probe.supplyRequired() / 2
		&& Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + reservedMinerals
		&& Broodwar->self()->gas() >= UnitTypes::Protoss_Probe.gasPrice() + reservedGas;
}

void ExampleAIModule::buildProbe(BWAPI::Unit u) {
	u->train(u->getType().getRace().getWorker());
	availableSupply -= investmentList[0].supplyRequired() / 2;
	probesQueued--;
	probesWarping++;
	investmentList.erase(investmentList.begin());
}

bool ExampleAIModule::gatewayNeeded() {
	return !gatewaysQueued && (gatewayCount + gatewaysQueued < nexusCount * 4 || Broodwar->self()->minerals() < MINERAL_SURPLUS_LIMIT + reservedMinerals);
}

bool ExampleAIModule::canBuildGateway() {
	if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Gateway.mineralPrice() + reservedMinerals
		&& Broodwar->self()->gas() >= UnitTypes::Protoss_Gateway.gasPrice() + reservedGas) {
		return true;
	}
	return false;
}

bool ExampleAIModule::zealotNeeded() {
	return !zealotsQueued;
}

/*void ExampleAIModule::buildGateway(BWAPI::Unit u) {
	//u->train(u->getType().getRace().getWorker());

	buildingsNeeded.push_back(UnitTypes::Protoss_Gateway);

	
		UnitType building = UnitTypes::Protoss_Gateway;
		static int lastCheck = 0;

		if (lastCheck + 400 < Broodwar->getFrameCount()) {

			lastCheck = Broodwar->getFrameCount();

			Unit builder = u->getClosestUnit(GetType == building.whatBuilds().first &&
				(IsIdle || IsGatheringMinerals) && IsOwned);

			if (builder) {
				TilePosition buildLocation = Broodwar->getBuildLocation(building, builder->getTilePosition());

				if (buildLocation) {

					// Box to display where the building is placed
					Broodwar->registerEvent([buildLocation, building](Game*) {
						Broodwar->drawBoxMap(Position(buildLocation),
							Position(buildLocation + building.tileSize()),
							Colors::Blue);
					}, nullptr,
						building.buildTime() + 100);

					// Build the building
					builder->build(building, buildLocation);

					gatewayCount++;
					reservedMinerals += 150;
				
			}
		}
	}
}*/

void ExampleAIModule::mineMinerals(BWAPI::Unit u) {
	if (u->isIdle()) {
		if (u->isCarryingMinerals()) {
			u->returnCargo();
		}
		else {
			u->gather(u->getClosestUnit(IsMineralField));
		}
	}
}

/*void ExampleAIModule::buildZealot(BWAPI::Unit gate){
	if (gate->isIdle()){
		gate->build(UnitTypes::Protoss_Zealot);
	}
}*/