#pragma once
#include "ExampleAIModule.h"

// BWTA2 variables
bool analyzed;
bool analyzedJustFinished;

using namespace BWAPI;
using namespace Filter;

// BWTA2 function for threading the analysis
DWORD WINAPI AnalyzeThread() {
	BWTA::analyze();

	analyzed = true;
	analyzedJustFinished = true;

	return 0;
}

void ExampleAIModule::onStart() {
	// Bot Setup
	Broodwar->enableFlag(Flag::UserInput); // Remove when bot is ready
	Broodwar->setCommandOptimizationLevel(2);

	// BWTA2 stuffs
	BWTA::readMap();
	analyzed = false;
	analyzedJustFinished = false;

	Broodwar->sendText("Analyzing map, slow if map has not loaded yet");
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);

	// Other onStart stuff
	scoutManager._init();

	strategyDecider._init(&tactician, &scoutManager);
	tactician._init(&scoutManager);
}

void ExampleAIModule::onEnd(bool isWinner) { }

void ExampleAIModule::onFrame() {
	// Debug drawing
	drawData();

	// When BWTA2 finishes reading the map, we can call all
	// classes that use the BWTA2 functions and say it is available
	if (analyzedJustFinished) {
		Broodwar->sendText("Finished analyzing map");
		analyzedJustFinished = false;
		strategyDecider.setAnalyzed();
	}

	// Skip game logic if it's not really 'us'
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self()) {
		return;
	}

	// Call our game logic
	strategyDecider.update();

}

void ExampleAIModule::onSendText(std::string text) {
	// Send the text to the game if it is not being processed.
	if (text == "/scoutall") {
		tactician.addAllScouts();
	}
	else {
		Broodwar->sendText("%s", text.c_str());
	}

}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text) {
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player) { }

void ExampleAIModule::onNukeDetect(BWAPI::Position target) { }

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit) {
	if (unit->getType() == UnitTypes::Protoss_Assimilator){
		Broodwar->sendText("mamb discovered %s", unit->getType().c_str());

	}
	// Is it one of our own units?
	if (Broodwar->self() == unit->getPlayer()) {
		tactician.addWarpingUnit(unit);


	}
	// Was it an enemy unit?
	else if (Broodwar->enemy() == unit->getPlayer()) {
		scoutManager.recordUnitDiscover(unit);
	}
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit) {
	if (Broodwar->enemy() == unit->getPlayer()) {
		scoutManager.recordUnitEvade(unit);
	}
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit) { }

void ExampleAIModule::onUnitHide(BWAPI::Unit unit) { }

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit) { }

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit) {
	// Is it one of our own units?
	if (Broodwar->self() == unit->getPlayer()) {
		Broodwar->sendText("%s er %s slettet", unit->getType().c_str(), tactician.recordDeadUnit(unit) ? "" : "IKKE!");
	}
	// Was it an enemy unit?
	else if (Broodwar->enemy() == unit->getPlayer()) {
		scoutManager.recordUnitDestroy(unit);
	}
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit) { }

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit) { }

void ExampleAIModule::onSaveGame(std::string gameName) { }

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit) {
	if (unit->getType() == UnitTypes::Protoss_Assimilator){
		Broodwar->sendText("MUnit Complete %s", unit->getType().c_str());

	}
	if (Broodwar->self() == unit->getPlayer()) {
		tactician.recordNewUnit(unit);
	}
}

// END OF CALLBACKS

// Now debug functions begin for drawing data
// on the map / screen
	

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
	
	//if (scoutManager.isScouting) {
	//	TilePosition::list scoutSpawns = scoutManager.getSpawns();
	//	std::vector<bool> scoutSpawnBools = scoutManager.getSpawnBools();
	//	std::vector<ScoutAndGoalStruct*> scouters = scoutManager.getScouts();
	//	for (u_int i = 0; i < scoutSpawns.size(); i++) {
	//		bool draw = scoutSpawnBools.at(i) ? 1 : 0;

	//		Position pos = Position(Broodwar->self()->getStartLocation());

	//		if (scoutManager.hasScouts()) {
	//			for (auto &lort : scouters) {
	//				if (lort->goal == Position(scoutSpawns.at(i))) {
	//					pos = lort->scout->getPosition();
	//				}
	//			}
	//		}


	//		Broodwar->drawTextScreen(20, 20 + 10 * i, "Spawn %i: (%d, %d), dist: %d, scouted: %d",
	//			i,
	//			Position(scoutSpawns.at(i)).x,
	//			//scoutSpawns.at(i).x,
	//			Position(scoutSpawns.at(i)).y,
	//			//scoutSpawns.at(i).y,

	//			Position(scoutSpawns.at(i)).getApproxDistance(pos),
	//			//Position(scoutSpawns.at(i)).getApproxDistance(Position(Broodwar->self()->getStartLocation())),
	//			//Position(scoutSpawns.at(i)).getApproxDistance(Position(Broodwar->self()->getStartLocation())),
	//			draw
	//			);
	//	}
	//}
	
	TilePosition tilePos = scoutManager.getEnemySpawn();
	Position pos = Position(tilePos);
	Broodwar->drawTextScreen(20, 10, "Enemy spawn: (%i, %i)", pos.x, pos.y);
	
	std::map<BWAPI::UnitType, int> maps = scoutManager.getEnemyUnitsAmount();

	std::map<BWAPI::UnitType, int>::iterator it;

	int i = 0;
	for (it = maps.begin(); it != maps.end(); it++) {
		Broodwar->drawTextScreen(
				20, 20 + 10 * i,
				"UnitType %s, amount %i",
				it->first.c_str(),
				it->second
			);

		i++;
	}


}