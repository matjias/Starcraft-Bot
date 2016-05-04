#pragma once
#include "ExampleAIModule.h"

using namespace BWAPI;
using namespace Filter;

// BWTA variables, need to be here
bool analyzed;
bool analysis_just_finished;


void ExampleAIModule::onStart() {
	// Bot Setup
	Broodwar->enableFlag(Flag::UserInput); // Remove when bot is ready
	Broodwar->setCommandOptimizationLevel(2);

	// BWTA2 stuffs
	BWTA::readMap();
	analyzed = false;
	analysis_just_finished = false;

	Broodwar << "Analyzing map... this may take a minute" << std::endl;;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);

	// Other onStart stuff

}

void ExampleAIModule::onEnd(bool isWinner) { }

void ExampleAIModule::onFrame() {
	drawData();

	// BWTA2 drawing on the map
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	if (analysis_just_finished) {
		Broodwar << "Finished analyzing map." << std::endl;;
		analysis_just_finished = false;
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

void ExampleAIModule::onPlayerLeft(BWAPI::Player player) { }

void ExampleAIModule::onNukeDetect(BWAPI::Position target) { }

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit) {

	}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit) { }

	if (unit->getType() == UnitTypes::Protoss_Zealot && unit->getPlayer() == Broodwar->self()){
		army.addZealot(unit);
	}

	if (unit->getType() == UnitTypes::Protoss_Dragoon && unit->getPlayer() == Broodwar->self()){

		army.addDragoon(unit);
	}

	if (unit->getType() == UnitTypes::Protoss_Observer && unit->getPlayer() == Broodwar->self()){
		//army.addObserver(unit);
	}

	if (unit->getType() == UnitTypes::Protoss_Corsair && unit->getPlayer() == Broodwar->self()){
		//army.addCorsair(unit);
	}

	if (unit->getPlayer() != Broodwar->self() && unit->getType().isResourceDepot()) {
		scoutClass.foundEnemyBase(TilePosition(unit->getPosition()));
	}

	if (unit->getPlayer() != Broodwar->self() && !unit->getType().isNeutral()) {
		scoutClass.recordUnitDiscover(unit, TilePosition(unit->getPosition()), Broodwar->getFrameCount());
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

}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit) { }

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit) { }

void ExampleAIModule::onSaveGame(std::string gameName) { }

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit) {

}

// END OF CALLBACKS


// BWTA2 functions
DWORD WINAPI AnalyzeThread() {
	BWTA::analyze();

	analyzed = true;
	analysis_just_finished = true;

	// Tell any classes here that BWTA has finished

	
	return 0;
}

bool ExampleAIModule::isAnalyzed(){
	return analyzed;
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

void ExampleAIModule::drawData() {
	// Highlight possible enemy expansions (work in progress)
	std::vector<BWTA::BaseLocation*> expansions = scoutClass.closestEnemyExpansions();
	for (auto &exp : expansions) {
		Broodwar->drawCircleMap(exp->getPosition(), TILE_SIZE * 3, Colors::Teal);
	}

	//Broodwar->drawTextScreen(20, 40, "Highlighted %i expansions", expansions.size());

	Broodwar->drawTextScreen(500, 30, "FPS: %d", Broodwar->getFPS());
	//Broodwar->drawTextScreen(500, 40, "Average FPS: %f", Broodwar->getAverageFPS());
	Broodwar->drawTextScreen(500, 40, "%s", Broodwar->enemy()->getRace().c_str());
	Broodwar->drawTextScreen(500, 50, "Zealot rate: %f", buildOrderClass.zealotRate);
	Broodwar->drawTextScreen(500, 60, "Dragoon rate: %f", buildOrderClass.dragoonRate);
	Broodwar->drawTextScreen(500, 70, "All-in: %s", buildOrderClass.getAllIn() ? "true" : "false");

	/*
	std::map<UnitType, Scouting::UnitStruct*> enemyUnits = scoutClass.getEnemyUnits();
	int debugCount = 0;
	for (std::map<UnitType, Scouting::UnitStruct*>::iterator iterator = enemyUnits.begin();
		iterator != enemyUnits.end(); iterator++) {
		
		Broodwar->drawTextScreen(20, 60 + debugCount * 20, "%s, %i",
		iterator->first.c_str(),
		iterator->second->unit.size());

		debugCount++;
	}
	*/

	//Broodwar->drawTextScreen(200, 20, "Available Supply: %d + %d", availableSupply, supplyBuffer);

	for (int i = 0; i < buildOrderClass.getInvestmentList().size(); i++) {
		Broodwar->drawTextScreen(5, 5 + i * 10, "%i: %s", i, buildOrderClass.getInvestmentList().at(i).c_str());
	}

	/*if (assimilators.size() == 0) {
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
	}*/

	std::map<TilePosition, Scouting::BuildingStruct*, Scouting::CustomMapCompare> enemyStructs = scoutClass.getEnemyStructures();
	int debugCount = 0;
	for (std::map<TilePosition, Scouting::BuildingStruct*, Scouting::CustomMapCompare>::iterator iterator = enemyStructs.begin();
		iterator != enemyStructs.end(); iterator++) {
		/*Broodwar->drawTextScreen(5, 60 + debugCount * 20, "%s, (%i,%i), %i",
			iterator->second->unit.c_str(),
			Position(iterator->second->location).x, Position(iterator->second->location).y,
			iterator->second->scoutedTime);
			*/

		TilePosition p = iterator->first;
		UnitType u = iterator->second->unit;

		Position topLeft = Position(TILE_SIZE * (p.x - u.tileWidth() / 2),
			TILE_SIZE * (p.y - u.tileHeight() / 2));
		Position botRight = Position(topLeft.x + u.tileWidth() * TILE_SIZE,
			topLeft.y + u.tileHeight() * TILE_SIZE);
		Broodwar->drawBoxMap(topLeft, botRight, Colors::Red);

		debugCount++;
	}




	//for (int i = 0; i < enemyStructs.size(); i++) {
	//	Broodwar->drawTextScreen(5, 60 + i * 20, "%s, (%i,%i), %i", enemyStructs.at(i))
	//}

	Broodwar->drawTextScreen(350, 20, "Scout distance: %i", scoutClass.getDistance());

	Position scoutClassPos = scoutClass.getPos();
	Broodwar->drawTextScreen(350, 40, "Location: %i, %i", scoutClassPos.x, scoutClassPos.y);

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

	Broodwar->drawTextScreen(5, 20 * i + 100, "Spawn %i, {%d, %d} dist: %d, dist to scout: %d, scouted: %d",
	i + 1, Position(spawns.at(i)).x, Position(spawns.at(i)).y,
	Position(spawns.at(i)).getApproxDistance(Position(Broodwar->self()->getStartLocation())),
	scoutClass.getScout().getApproxDistance(Position(spawns.at(i))),
	toDraw);
	}*/

	/*Broodwar->drawTextScreen(200, 0, "Reserved Min: %d", buildOrderClass.getReservedMinerals());
	Broodwar->drawTextScreen(200, 10, "Reserved Gas: %d", buildOrderClass.getReservedGas());
	Broodwar->drawTextScreen(200, 20, "AvailableSupply: %d", buildOrderClass.getAvailableSupply());
	Broodwar->drawTextScreen(200, 30, "Nexuses: %d, W: %d, Q: %d", buildOrderClass.nexuses, buildOrderClass.nexusesWarping, buildOrderClass.nexusesQueued);
	Broodwar->drawTextScreen(200, 40, "Pylons: %d, W: %d, Q: %d", buildOrderClass.pylons, buildOrderClass.pylonsWarping, buildOrderClass.pylonsQueued);
	Broodwar->drawTextScreen(200, 50, "Gateways: %d, W: %d, Q: %d", buildOrderClass.gateways, buildOrderClass.gatewaysWarping, buildOrderClass.gatewaysQueued);
	Broodwar->drawTextScreen(200, 60, "CyberCores: %d, W: %d, Q: %d", buildOrderClass.cyberneticsCores, buildOrderClass.cyberneticsCoresWarping, buildOrderClass.cyberneticsCoresQueued);
	Broodwar->drawTextScreen(200, 70, "Assimilators: %d, W: %d, Q: %d", buildOrderClass.assimilators, buildOrderClass.assimilatorsWarping, buildOrderClass.assimilatorsQueued);
	Broodwar->drawTextScreen(200, 80, "RoboFacils: %d, W: %d, Q: %d", buildOrderClass.roboticsFacilities, buildOrderClass.roboticsFacilitiesWarping, buildOrderClass.roboticsFacilitiesQueued);
	Broodwar->drawTextScreen(200, 90, "Observatories: %d, W: %d, Q: %d", buildOrderClass.observatories, buildOrderClass.observatoriesWarping, buildOrderClass.observatoriesQueued);
	Broodwar->drawTextScreen(200, 100, "Stargates: %d, W: %d, Q: %d", buildOrderClass.stargates, buildOrderClass.stargatesWarping, buildOrderClass.stargatesQueued);

	Broodwar->drawTextScreen(200, 110, "Probes: %d, W: %d, Q: %d", buildOrderClass.probes, buildOrderClass.probesWarping, buildOrderClass.probesQueued);
	Broodwar->drawTextScreen(200, 120, "Zealots: %d, W: %d, Q: %d", buildOrderClass.zealots, buildOrderClass.zealotsWarping, buildOrderClass.zealotsQueued);
	Broodwar->drawTextScreen(200, 130, "Dragoons: %d, W: %d, Q: %d", buildOrderClass.dragoons, buildOrderClass.dragoonsWarping, buildOrderClass.dragoonsQueued);
	Broodwar->drawTextScreen(200, 140, "Observers: %d, W: %d, Q: %d", buildOrderClass.observers, buildOrderClass.observersWarping, buildOrderClass.observersQueued);
	Broodwar->drawTextScreen(200, 150, "Corsairs: %d, W: %d, Q: %d", buildOrderClass.corsairs, buildOrderClass.corsairsWarping, buildOrderClass.corsairsQueued);*/

	//BWTA draw
	if (analyzed) {
		drawTerrainData();
	}
}