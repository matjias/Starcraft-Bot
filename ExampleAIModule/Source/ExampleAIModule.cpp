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

// BWTA2 variable
bool analyzed;
bool analysis_just_finished;

Unit builder;

int stepsWaited;

std::vector<Unit> probesMiningGas;
std::vector<Unit> assimilators;
std::vector<Unit> gateways;

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

	// BWTA2 stuffs
	BWTA::readMap();
	analyzed = false;
	analysis_just_finished = false;

	Broodwar << "Analyzing map... this may take a minute" << std::endl;;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);

	army._init();
}

void ExampleAIModule::onEnd(bool isWinner) {
	
}



void ExampleAIModule::onFrame() {
	//Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	//Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	//Broodwar->drawTextScreen(200, 20, "Available Supply: %d + %d", availableSupply, supplyBuffer);
	//Broodwar->drawTextScreen(200, 40, "Gateways: %d", gatewayCount);
	
	for (int i = 0; i < buildOrderClass.getInvestmentList().size(); i++) {
		Broodwar->drawTextScreen(5, 5 + i * 20, "%i: %s", i, buildOrderClass.getInvestmentList().at(i).c_str());
	}

	if (assimilators.size() == 0) {
		Broodwar->drawTextScreen(200, 5, "%i: %s", 0, "No Assimilators");
		Broodwar->drawTextScreen(210, 25, "%i: %s", 0, "No Probes harvesting gas");
	}

	for (int i = 0; i < assimilators.size(); i++) {
		Broodwar->drawTextScreen(200, 5 + i * 20, "%i: %s", i, assimilators.at(i)->getType().c_str());
	}

	for (int i = 0; i < probesMiningGas.size(); i++) {
		if (probesMiningGas.at(i) != 0) {
			Broodwar->drawTextScreen(210, 5 + i * 20 + assimilators.size() * 20, "%i: %s", i, probesMiningGas.at(i)->getType().c_str());
		}
		else {
			Broodwar->drawTextScreen(210, 5 + i * 20 + assimilators.size() * 20, "%i: %s", i, "-");
		}
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
	*//*
	TilePosition::list spawns = scoutClass.getDynamicSpawns();
	std::vector<bool> spawnsBool = scoutClass.getDynamicSpawnBools();
	for (int i = 0; i < spawns.size(); i++) {
		bool toDraw = spawnsBool.at(i) ? 1 : 0;

		Broodwar->drawTextScreen(5, 20 * i + 20, "Spawn %i, {%d, %d} dist: %d, dist to scout: %d, scouted: %d",
			i + 1, Position(spawns.at(i)).x, Position(spawns.at(i)).y,
			Position(spawns.at(i)).getApproxDistance(Position(Broodwar->self()->getStartLocation())),
			scoutClass.getScout().getApproxDistance(Position(spawns.at(i))),
			toDraw);
	}*/
	
	//Broodwar->drawTextScreen(20, 0, "Reserved Min: %d", reservedMinerals);
	//Broodwar->drawTextScreen(20, 20, "Reserved Gas: %d", reservedGas);
	//Broodwar->drawTextScreen(20, 40, "Nexuses: %d, W: %d, Q: %d", nexusCount, nexusesWarping, nexusesQueued);
	//Broodwar->drawTextScreen(20, 60, "Pylons: %d, W: %d, Q: %d", pylonCount, pylonsWarping, pylonsQueued);
	//Broodwar->drawTextScreen(20, 80, "Gateways: %d, W: %d, Q: %d", gatewayCount, gatewaysWarping,  gatewaysQueued);
	//Broodwar->drawTextScreen(20, 100, "Workers: %d, W: %d, Q: %d", probeCount, probesWarping, probesQueued);


	// BWTA2 drawing on the map
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	//BWTA draw
	if (analyzed)
		drawTerrainData();

	if (analysis_just_finished)
	{
		Broodwar << "Finished analyzing map." << std::endl;;
		analysis_just_finished = false;
	}

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;
	// End of BWTA2 draw calls


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
			if (!scoutClass.isScouting() && !scoutClass.returnFoundEnemyBase()) {
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

			// Gather resources
			else if (!scoutClass.isScout(u)) {
				if ((u->isCarryingMinerals() || u->isCarryingGas()) && u->isIdle()) {
					u->returnCargo();
				}
				else if (gasGathererNeeded()
					&& !isGatheringGas(u)
					&& builder != u) {
					mineGas(u);
				}
				else {
					if (u->isIdle()) {
						mineMinerals(u);
					}
				}
			}
		}

		// Nexus logic
		else if (u->getType().isResourceDepot()
			&& u->isIdle()
			&& !buildOrderClass.getInvestmentList().empty()
			&& buildOrderClass.getInvestmentList()[0] == UnitTypes::Protoss_Probe
			&& Broodwar->self()->minerals() >= buildOrderClass.getInvestmentList()[0].mineralPrice() + buildOrderClass.getReservedMinerals()
			&& Broodwar->self()->gas() >= buildOrderClass.getInvestmentList()[0].gasPrice() + buildOrderClass.getReservedGas()
			&& buildOrderClass.getAvailableSupply() >= buildOrderClass.getInvestmentList()[0].supplyRequired() / 2) {
			buildProbe(u);
		}

		// Gateway logic
		else if (u->getType() == UnitTypes::Protoss_Gateway
			&& u->isIdle()
			&& !buildOrderClass.getInvestmentList().empty()
			&& Broodwar->self()->minerals() >= buildOrderClass.getInvestmentList()[0].mineralPrice() + buildOrderClass.getReservedMinerals()
			&& Broodwar->self()->gas() >= buildOrderClass.getInvestmentList()[0].gasPrice() + buildOrderClass.getReservedGas()
			&& buildOrderClass.getAvailableSupply() >= buildOrderClass.getInvestmentList()[0].supplyRequired() / 2) {
			if (buildOrderClass.getInvestmentList()[0] == UnitTypes::Protoss_Zealot) {
				if (army.buildZealot(u)) {
					buildOrderClass.setAvailableSupply(buildOrderClass.getAvailableSupply() - buildOrderClass.getInvestmentList()[0].supplyRequired() / 2);
					buildOrderClass.zealotsQueued--;
					buildOrderClass.zealotsWarping++;
					buildOrderClass.dequeueInvestmentList();
				}
			}
			else if (buildOrderClass.getInvestmentList()[0] == UnitTypes::Protoss_Dragoon) {
				if (army.buildDragoon(u)) {
					buildOrderClass.setAvailableSupply(buildOrderClass.getAvailableSupply() - buildOrderClass.getInvestmentList()[0].supplyRequired() / 2);
					buildOrderClass.dragoonsQueued--;
					buildOrderClass.dragoonsWarping++;
					buildOrderClass.dequeueInvestmentList();
				}
			}
			
		}
		
		// Zealot attack logic
		//army.update(scoutClass);
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

	if (unit->getType() == UnitTypes::Protoss_Zealot && unit->getPlayer() == Broodwar->self()){
		army.addZealot(unit);
	}

	if (unit->getType() == UnitTypes::Protoss_Dragoon && unit->getPlayer() == Broodwar->self()){
		army.addDragoon(unit);
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
		for (int i; i < probesMiningGas.size(); i++) {
			if (probesMiningGas.at(i) == unit) {
				probesMiningGas.at(i) = 0;
			}
		}
	}
	else if (unit->getType() == UnitTypes::Protoss_Zealot && unit->getPlayer() == Broodwar->self()) {
		buildOrderClass.zealotCount--;
	}
	else if (unit->getType() == UnitTypes::Protoss_Assimilator && unit->getPlayer() == Broodwar->self()) {
		buildOrderClass.assimilatorCount--;
		for (int i; i < assimilators.size(); i++) {
			if (assimilators.at(i) == unit) {
				assimilators.erase(assimilators.begin() + i);
				for (int j; j < 3; j++) {
					probesMiningGas.erase(assimilators.begin() + i * 3);
				}
			}
		}
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
	else if (unit->getType() == UnitTypes::Protoss_Gateway && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.gatewaysWarping--;
		buildOrderClass.gatewayCount++;
	}
	else if (unit->getType() == UnitTypes::Protoss_Assimilator && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.assimilatorsWarping--;
		buildOrderClass.assimilatorCount++;
		assimilators.push_back(unit);
		probesMiningGas.push_back(0);
		probesMiningGas.push_back(0);
		probesMiningGas.push_back(0);
	}
	else if (unit->getType() == UnitTypes::Protoss_Probe && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.probesWarping--;
		buildOrderClass.probeCount++;
	}
	else if (unit->getType() == UnitTypes::Protoss_Zealot && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.zealotsWarping--;
		buildOrderClass.zealotCount++;
	}
	else if (unit->getType() == UnitTypes::Protoss_Dragoon && unit->getPlayer() == Broodwar->self() && Broodwar->elapsedTime() > 1) {
		buildOrderClass.zealotsWarping--;
		buildOrderClass.dragoonCount++;
	}
}

// BWTA2 functions
DWORD WINAPI AnalyzeThread() {
	BWTA::analyze();

	analyzed = true;
	analysis_just_finished = true;
	army.setAnalyzed(true);
	return 0;
}

void ExampleAIModule::drawTerrainData() {
	//we will iterate through all the base locations, and draw their outlines.
	for (const auto& baseLocation : BWTA::getBaseLocations()) {
		TilePosition p = baseLocation->getTilePosition();

		//draw outline of center location
		Position leftTop(p.x * TILE_SIZE, p.y * TILE_SIZE);
		Position rightBottom(leftTop.x + 4 * TILE_SIZE, leftTop.y + 3 * TILE_SIZE);
		Broodwar->drawBoxMap(leftTop, rightBottom, Colors::Blue);

		//draw a circle at each mineral patch
		for (const auto& mineral : baseLocation->getStaticMinerals()) {
			Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, Colors::Cyan);
		}

		//draw the outlines of Vespene geysers
		for (const auto& geyser : baseLocation->getGeysers()) {
			TilePosition p1 = geyser->getInitialTilePosition();
			Position leftTop1(p1.x * TILE_SIZE, p1.y * TILE_SIZE);
			Position rightBottom1(leftTop1.x + 4 * TILE_SIZE, leftTop1.y + 2 * TILE_SIZE);
			Broodwar->drawBoxMap(leftTop1, rightBottom1, Colors::Orange);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if (baseLocation->isIsland()) {
			Broodwar->drawCircleMap(baseLocation->getPosition(), 80, Colors::Yellow);
		}
	}

	//we will iterate through all the regions and ...
	for (const auto& region : BWTA::getRegions()) {
		// draw the polygon outline of it in green
		BWTA::Polygon p = region->getPolygon();
		for (size_t j = 0; j < p.size(); ++j) {
			Position point1 = p[j];
			Position point2 = p[(j + 1) % p.size()];
			Broodwar->drawLineMap(point1, point2, Colors::Green);
		}
		// visualize the chokepoints with red lines
		for (auto const& chokepoint : region->getChokepoints()) {
			Position point1 = chokepoint->getSides().first;
			Position point2 = chokepoint->getSides().second;
			Broodwar->drawLineMap(point1, point2, Colors::Red);
		}
	}
}
// End of BWTA2 functions

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

void ExampleAIModule::mineMinerals(BWAPI::Unit u) {
	u->gather(u->getClosestUnit(IsMineralField));
}

void ExampleAIModule::mineGas(BWAPI::Unit u) {
	for (int i = 0; i < probesMiningGas.size(); i++) {
		if (probesMiningGas.at(i) == 0) {
			probesMiningGas.at(i) = u;
			u->gather(assimilators.at(i / WORKERS_PER_GEYSER));
			break;
		}
	}
}

bool ExampleAIModule::gasGathererNeeded() {
	if (buildOrderClass.assimilatorCount > 0 && buildOrderClass.probeCount >= buildOrderClass.assimilatorCount * WORKERS_PER_GEYSER * 2) {
		for (int i = 0; i < probesMiningGas.size(); i++) {
			if (probesMiningGas.at(i) == 0) {
				return true;
			}
		}
	}
	return false;
}

bool ExampleAIModule::isGatheringGas(BWAPI::Unit u) {
	for (int i = 0; i < probesMiningGas.size(); i++) {
		if (probesMiningGas.at(i) == u) {
			return true;
		}
	}
	return false;
}

bool isAnalyzed(){
	return analyzed;
}
