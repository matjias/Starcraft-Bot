#pragma once
#include "ExampleAIModule.h"
#include "Constants.h"
#include "Scouting.h"
#include "BuildOrders.h"
#include "Army.h"
#include <iostream>
#include <vector>

using namespace BWAPI;
using namespace Filter;

Unit builder;

int stepsWaited;

std::vector<Unit>gateways;

BuildOrders buildOrderClass;
Scouting scoutClass;

Army army;

void ExampleAIModule::onStart() {
	Broodwar->enableFlag(Flag::UserInput);
	Broodwar->setCommandOptimizationLevel(2);
	
	stepsWaited = 0;

	builder = 0;

	buildOrderClass.setAvailableSupply((Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2);

	// Scouting stuff
	scoutClass._init(Broodwar->getStartLocations(), Broodwar->self()->getStartLocation());
}

void ExampleAIModule::onEnd(bool isWinner) {
	
}



void ExampleAIModule::onFrame() {
	//Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	//Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	//Broodwar->drawTextScreen(200, 20, "Available Supply: %d + %d", availableSupply, supplyBuffer);
	//Broodwar->drawTextScreen(200, 40, "Gateways: %d", gatewayCount);
	
	for (int i = 0; i < buildOrderClass.getInvestmentList().size(); i++) {
		Broodwar->drawTextScreen(5, 60 + i * 20, "%i: %s", i, buildOrderClass.getInvestmentList().at(i).c_str());
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
	buildOrderClass.setAvailableSupply((Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2);

	// Scout Class handles everything itself, anything
	// it finds will pop up in onUnitDiscover, we just
	// have to check if it is the enemy's
	scoutClass.updateScout();
	
	// Use initial build order
	if (!buildOrderClass.getFirstBuildOrderFinished()) {
		buildOrderClass.useInitialBuildOrder();
	}

	// Use appropriate second build order
	else if (!buildOrderClass.getSecondBuildOrderFinished()) {
		buildOrderClass.useSecondBuildOrder();
	}

	// Add resource invesments
	else {
		buildOrderClass.addInvestments();

		// Reorder investment priorities
		buildOrderClass.reorderInvestments();
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
				&& !buildOrderClass.getInvestmentList().empty()
				&& buildOrderClass.getInvestmentList()[0].isBuilding()
				&& Broodwar->self()->minerals() >= buildOrderClass.getInvestmentList()[0].mineralPrice() + buildOrderClass.getReservedMinerals()
				&& Broodwar->self()->gas() >= buildOrderClass.getInvestmentList()[0].gasPrice() + buildOrderClass.getReservedGas()
				&& (u->isIdle() || u->isGatheringMinerals() || u->isGatheringGas())) {
				buildBuilding(u, buildOrderClass.getInvestmentList()[0]);
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
			&& !buildOrderClass.getInvestmentList().empty()
			&& buildOrderClass.getInvestmentList()[0] == UnitTypes::Protoss_Probe
			&& Broodwar->self()->minerals() >= buildOrderClass.getInvestmentList()[0].mineralPrice() + buildOrderClass.getReservedMinerals()
			&& Broodwar->self()->gas() >= buildOrderClass.getInvestmentList()[0].gasPrice() + buildOrderClass.getReservedGas()
			&& buildOrderClass.getAvailableSupply() >= buildOrderClass.getInvestmentList()[0].supplyRequired() / 2) {
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
			&& !buildOrderClass.getInvestmentList().empty()
			&& buildOrderClass.getInvestmentList()[0] == UnitTypes::Protoss_Zealot
			&& Broodwar->self()->minerals() >= buildOrderClass.getInvestmentList()[0].mineralPrice() + buildOrderClass.getReservedMinerals()
			&& Broodwar->self()->gas() >= buildOrderClass.getInvestmentList()[0].gasPrice() + buildOrderClass.getReservedGas()
			&& buildOrderClass.getAvailableSupply() >= buildOrderClass.getInvestmentList()[0].supplyRequired() / 2) {
			//(availableSupply > 1 || Broodwar->self()->incompleteUnitCount(u->getType().getRace().getSupplyProvider()) > 0)) {
			if (army.buildZealot(u)){
				buildOrderClass.setAvailableSupply(buildOrderClass.getAvailableSupply() - buildOrderClass.getInvestmentList()[0].supplyRequired() / 2);
				buildOrderClass.zealotsQueued--;
				buildOrderClass.zealotsWarping++;
				buildOrderClass.dequeueInvestmentList();
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
		buildOrderClass.setReservedMinerals(buildOrderClass.getReservedMinerals() - unit->getType().mineralPrice());
		buildOrderClass.setReservedGas(buildOrderClass.getReservedGas() - unit->getType().gasPrice());
	}

	if (unit->getType() == UnitTypes::Protoss_Pylon && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.pylonsQueued--;
		buildOrderClass.pylonsWarping++;
	}
	if (unit->getType() == UnitTypes::Protoss_Gateway && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.gatewaysQueued--;
		buildOrderClass.gatewaysWarping++;
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
		buildOrderClass.probeCount--;
	}
	else if (unit->getType() == UnitTypes::Protoss_Zealot && unit->getPlayer() == Broodwar->self()) {
		buildOrderClass.zealotCount--;
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
		buildOrderClass.setSupplyBuffer(buildOrderClass.getSupplyBuffer() - unit->getType().supplyProvided() / 2);
		buildOrderClass.pylonsWarping--;
		buildOrderClass.pylonCount++;
	}
	if (unit->getType() == UnitTypes::Protoss_Gateway && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.gatewaysWarping--;
		buildOrderClass.gatewayCount++;
	}
	if (unit->getType() == UnitTypes::Protoss_Probe && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.probesWarping--;
		buildOrderClass.probeCount++;
	}
	if (unit->getType() == UnitTypes::Protoss_Zealot && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.zealotsWarping--;
		buildOrderClass.zealotCount++;
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

		buildOrderClass.setReservedMinerals(buildOrderClass.getReservedMinerals() + building.mineralPrice());

		buildOrderClass.dequeueInvestmentList();
	}
}

bool ExampleAIModule::canBuildSupply() {
	if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Pylon.mineralPrice() + buildOrderClass.getReservedMinerals()
		&& Broodwar->self()->gas() >= UnitTypes::Protoss_Pylon.gasPrice() + buildOrderClass.getReservedGas()) {
		for (int i = 0; i < buildOrderClass.getInvestmentList().size(); i++) {
			if (buildOrderClass.getInvestmentList()[i] == UnitTypes::Protoss_Pylon) {
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

bool ExampleAIModule::canbuildProbe() {
	return buildOrderClass.getAvailableSupply() >= UnitTypes::Protoss_Probe.supplyRequired() / 2
		&& Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + buildOrderClass.getReservedMinerals()
		&& Broodwar->self()->gas() >= UnitTypes::Protoss_Probe.gasPrice() + buildOrderClass.getReservedGas();
}

void ExampleAIModule::buildProbe(BWAPI::Unit u) {
	u->train(u->getType().getRace().getWorker());
	buildOrderClass.setAvailableSupply(buildOrderClass.getAvailableSupply() - buildOrderClass.getInvestmentList()[0].supplyRequired() / 2);
	buildOrderClass.probesQueued--;
	buildOrderClass.probesWarping++;
	buildOrderClass.dequeueInvestmentList();
}

bool ExampleAIModule::canBuildGateway() {
	if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Gateway.mineralPrice() + buildOrderClass.getReservedMinerals()
		&& Broodwar->self()->gas() >= UnitTypes::Protoss_Gateway.gasPrice() + buildOrderClass.getReservedGas()) {
		return true;
	}
	return false;
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
