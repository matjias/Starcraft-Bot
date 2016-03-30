#pragma once
#include "ExampleAIModule.h"
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


const int MAX_SUPPLY = 200;
const int MAX_WORKERS = 100;
const int MINERAL_SURPLUS_LIMIT = 1000;
const int GAS_SURPLUS_LIMIT = 1000;
const int ZEALOT_RUSH_SIZE = 1;

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

std::vector<UnitType>buildOrder;
std::vector<UnitType>investmentList;

bool firstBuildOrderStarted;
bool firstBuildOrderFinished;
bool secondBuildOrderStarted;
bool secondBuildOrderFinished;

std::vector<Unit>gateways;

BuildOrders buildOrderClass;
Scouting scoutClass;

Army army;

void ExampleAIModule::onStart() {
	Broodwar->enableFlag(Flag::UserInput);
	Broodwar->setCommandOptimizationLevel(2);

	stepsWaited = 0;

	nexusCount = 1;
	probeCount = 4;
	builder = 0;

	firstBuildOrderStarted = false;
	firstBuildOrderFinished = false;
	secondBuildOrderStarted = false;
	secondBuildOrderFinished = false;

	availableSupply = (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2;

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
	
	for (int i = 0; i < investmentList.size(); i++) {
		//Broodwar->drawTextScreen(5, 60 + i * 20, "%i: %s", i, investmentList.at(i).c_str());
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
	availableSupply = (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2;

	// Scout Class handles everything itself, anything
	// it finds will pop up in onUnitDiscover, we just
	// have to check if it is the enemy's
	scoutClass.updateScout();
	
	// Use initial build order
	if (!firstBuildOrderFinished) {
		if (investmentList.empty()) {
			if (firstBuildOrderStarted) {
				firstBuildOrderFinished = true;
			}
			else {
				firstBuildOrderStarted = true;
				investmentList = buildOrderClass.buildOrderInitial;
				updateQueueValues();
				
			}
		}
	}

	// Use appropriate second build order
	else if (!secondBuildOrderFinished) {
		if (investmentList.empty()) {
			if (secondBuildOrderStarted) {
				secondBuildOrderFinished = true;
			}
			else {
				secondBuildOrderStarted = true;
				investmentList = buildOrderClass.buildOrder2Gate;
				updateQueueValues();
			}
		}
	}

	// Add resource invesments
	else {
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
		army.update(scoutClass);
	}
}

void ExampleAIModule::onSendText(std::string text) {
	if (text == "/analyze") {
		if (analyzed == false) {
			Broodwar << "Analyzing map... this may take a minute" << std::endl;;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
		}
	}
	else {
		// Send the text to the game if it is not being processed.
		Broodwar->sendText("%s", text.c_str());
	}
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

void ExampleAIModule::updateQueueValues() {
	for (int i = 0; i < investmentList.size(); i++) {

		if (investmentList[i] == UnitTypes::Protoss_Nexus) {
			nexusesQueued++;
			supplyBuffer += UnitTypes::Protoss_Nexus.supplyProvided() / 2;
		}
		else if (investmentList[i] == UnitTypes::Protoss_Pylon) {
			pylonsQueued++;
			supplyBuffer += UnitTypes::Protoss_Pylon.supplyProvided() / 2;
		}
		else if (investmentList[i] == UnitTypes::Protoss_Gateway) {
			gatewaysQueued++;
		}
		else if (investmentList[i] == UnitTypes::Protoss_Probe) {
			probesQueued++;
		}
		else if (investmentList[i] == UnitTypes::Protoss_Zealot) {
			zealotsQueued++;
		}
	}
}

bool isAnalyzed(){
	return analyzed;
}