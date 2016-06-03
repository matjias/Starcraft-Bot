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
	TilePosition::list allSpawns = Broodwar->getStartLocations();
	TilePosition ownSpawn = Broodwar->self()->getStartLocation();
	ScoutManager._init(allSpawns, ownSpawn);

	StrategyDecider._init(&Tactician, &ScoutManager);
	Tactician._init(&ScoutManager);
}

void ExampleAIModule::onEnd(bool isWinner) { }

void ExampleAIModule::onFrame() {
	// Debug drawing
	drawData();

	// BWTA2 drawing on the map
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	if (analysis_just_finished) {
		Broodwar << "Finished analyzing map." << std::endl;;
		analysis_just_finished = false;
	}

	// Call our game logic
	StrategyDecider.update();

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
	// Is it one of our own units?
	if (Broodwar->self() == unit->getPlayer()) {
		// Tactician.addNewUnit(unit);

		//Broodwar->sendText("Own unit discovered: %s\n", unit->getType().c_str());

	}
	// Was it an enemy unit?
	else if (Broodwar->enemy() == unit->getPlayer()) {
		ScoutManager.recordUnitDiscover(unit);
	}
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit) {
	if (Broodwar->enemy() == unit->getPlayer()) {
		ScoutManager.recordUnitEvade(unit);
	}
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit) { }

void ExampleAIModule::onUnitHide(BWAPI::Unit unit) { }

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit) { }

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit) {
	// Is it one of our own units?
	if (Broodwar->self() == unit->getPlayer()) {
		Tactician.recordDeadUnit(unit);
	}
	// Was it an enemy unit?
	else if (Broodwar->enemy() == unit->getPlayer()) {
		ScoutManager.recordUnitDestroy(unit);
	}
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit) { }

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit) { }

void ExampleAIModule::onSaveGame(std::string gameName) { }

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit) {
	if (Broodwar->self() == unit->getPlayer()) {
		Tactician.recordNewUnit(unit);

		//Broodwar->sendText("Own unit completed: %s", unit->getType().c_str());
	}
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
	//BWTA draw
	if (analyzed) {
		drawTerrainData();
	}

	// ScoutManager debug
	TilePosition::list scoutSpawns = ScoutManager.getSpawns();
	std::vector<bool> scoutSpawnBools = ScoutManager.getSpawnBools();
	for (unsigned int i = 0; i < scoutSpawns.size(); i++) {
		bool draw = scoutSpawnBools.at(i) ? 1 : 0;

		Broodwar->drawTextScreen(20, 20 + 10 * i, "Spawn %i: (%d, %d), dist: %d, scouted: %d",
			i,
			//Position(scoutSpawns.at(i)).x,
			scoutSpawns.at(i).x,
			//Position(scoutSpawns.at(i)).y,
			scoutSpawns.at(i).y,
			Position(scoutSpawns.at(i)).getApproxDistance(Position(Broodwar->self()->getStartLocation())),
			draw
		);
	}

}