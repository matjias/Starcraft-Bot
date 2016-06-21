#pragma once
#include "ButcherBoy.h"

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

void ButcherBoy::onStart() {
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
	scoutManager.setStrategyDecider(&strategyDecider);
}

void ButcherBoy::onEnd(bool isWinner) { }

void ButcherBoy::onFrame() {
	// Debug drawing
	drawData();

	// When BWTA2 finishes reading the map, we can call all
	// classes that use the BWTA2 functions and say it is available
	if (analyzedJustFinished) {
		Broodwar->sendText("Finished analyzing map");
		analyzedJustFinished = false;
		strategyDecider.setAnalyzed();
		scoutManager.setAnalyzed();
	}

	// Skip game logic if it's not really 'us'
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self()) {
		return;
	}

	// Call our game logic
	strategyDecider.update();

}

void ButcherBoy::onSendText(std::string text) {
	// Send the text to the game if it is not being processed.
	if (text == "/scoutall") {
		tactician.addAllScouts();
	}
	else {
		Broodwar->sendText("%s", text.c_str());
	}

}

void ButcherBoy::onReceiveText(BWAPI::Player player, std::string text) {
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ButcherBoy::onPlayerLeft(BWAPI::Player player) { }

void ButcherBoy::onNukeDetect(BWAPI::Position target) { }

void ButcherBoy::onUnitDiscover(BWAPI::Unit unit) {
	// Is it one of our own units?
	if (Broodwar->self() == unit->getPlayer()) {
		tactician.addWarpingUnit(unit);
	}
	// Was it an enemy unit?
	else if (Broodwar->enemy() == unit->getPlayer()) {
		scoutManager.recordUnitDiscover(unit);
	}
}

void ButcherBoy::onUnitEvade(BWAPI::Unit unit) {
	if (Broodwar->enemy() == unit->getPlayer()) {
		scoutManager.recordUnitEvade(unit);
	}
}

void ButcherBoy::onUnitShow(BWAPI::Unit unit) { }

void ButcherBoy::onUnitHide(BWAPI::Unit unit) { }

void ButcherBoy::onUnitCreate(BWAPI::Unit unit) { }

void ButcherBoy::onUnitDestroy(BWAPI::Unit unit) {
	// Is it one of our own units?
	if (Broodwar->self() == unit->getPlayer()) {
		Broodwar->sendText("%s er %s slettet", unit->getType().c_str(), tactician.recordDeadUnit(unit) ? "" : "IKKE!");
	}
	// Was it an enemy unit?
	else if (Broodwar->enemy() == unit->getPlayer() ||
		unit->getType().isAddon()) {
		scoutManager.recordUnitDestroy(unit);
	}
}

void ButcherBoy::onUnitMorph(BWAPI::Unit unit) {
	if (Broodwar->self() == unit->getPlayer()) {
		// If one of our own units morphed, it must
		// have been the assimilator and we want to record that
		tactician.addWarpingUnit(unit);
	}
	else if (Broodwar->enemy() == unit->getPlayer()) {
		scoutManager.recordUnitMorph(unit);
	}
}

void ButcherBoy::onUnitRenegade(BWAPI::Unit unit) { }

void ButcherBoy::onSaveGame(std::string gameName) { }

void ButcherBoy::onUnitComplete(BWAPI::Unit unit) {
	if (Broodwar->self() == unit->getPlayer()) {
		tactician.recordNewUnit(unit);

		if (unit->getType() == UnitTypes::Protoss_Assimilator) {
			Broodwar->sendText("Assimilator completed");
		}
	}
}

// END OF CALLBACKS

// Now debug functions begin for drawing data
// on the map / screen
	

void ButcherBoy::drawTerrainData() {
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

void ButcherBoy::drawData() {
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

	Position posRZ = tactician.getRendezvousPos();
	if (posRZ != Position(0, 0)) {
		Broodwar->drawCircleMap(posRZ, TILE_SIZE, Colors::Red);
		Broodwar->drawTextMap(posRZ, "Rendezvous Point");
	}

	std::vector<TilePosition> path = tactician.getPathToEnemy();
	//Broodwar->sendText("Size of path %i", path.size());
	for (int j = 0; j < path.size(); j++) {
		if (j < path.size() - 1) {
			Broodwar->drawLineMap(Position(path.at(j)),
				Position(path.at(j + 1)),
				Colors::Cyan);
		}
	}

}