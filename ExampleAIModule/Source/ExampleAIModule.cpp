#pragma once
#include "ExampleAIModule.h"
#include "Scouting.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

const int MAX_SUPPLY = 200;
const int MAX_WORKERS = 100;

int availableSupply;
int reservedMinerals;
int reservedGas;

int workerCount;
int nexusCount;
int pylonCount;
int gatewayCount;

Scouting scoutClass;

void ExampleAIModule::onStart() {
	Broodwar->enableFlag(Flag::UserInput);
	Broodwar->setCommandOptimizationLevel(2);
	
	nexusCount = 1;


	// Scouting stuff
	scoutClass._init(Broodwar->getStartLocations(), Broodwar->self()->getStartLocation());
}

void ExampleAIModule::onEnd(bool isWinner) {
	
}

void ExampleAIModule::onFrame() {
	availableSupply = (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2;

	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	//Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	Broodwar->drawTextScreen(200, 20, "Available Supply: %d", availableSupply);
	//Broodwar->drawTextScreen(200, 60, "Worker Count: %d", workerCount);
	//Broodwar->drawTextScreen(200, 40, "Gateways: %d", gatewayCount);
	Broodwar->drawTextScreen(200, 40, "Reserved minerals: %d", reservedMinerals);
	Broodwar->drawTextScreen(350, 20, "Scout distance: %i", scoutClass.getDistance());
	Broodwar->drawTextScreen(350, 40, "Location: %i, %i", scoutClass.getX(), scoutClass.getY());

	Broodwar->drawTextScreen(200, 60, "Found Enemy: %d", scoutClass.returnFoundEnemyBase());
	Broodwar->drawTextScreen(350, 60, "Location: %i, %i", scoutClass.returnEnemyBaseLocs().x, scoutClass.returnEnemyBaseLocs().y);



	
	// AI Logic goes here

	// All Units loop
	for (auto &u : Broodwar->self()->getUnits()) {
		// If unit does not exist or is still being made, we ignore it
		if (!u->exists() || !u->isCompleted()) {
			continue;
		}

		// Worker logic
		// Currently only mining minerals
		if (u->getType().isWorker()) {
			if (!scoutClass.isScouting()) {
				scoutClass.assignScout(u);
			}



			// Gotta mine dem bitcoins
			if (u->isIdle()) {
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

			/*if (nexusDestroyd && !baseUnderAttack) {
				rebuildNexus();
			}*/
			
			if (supplyNeeded(u)) {
				buildSupply(u);
			}

			if (canBuildWorker(u) && workerNeeded(u)) {
				buildWorker(u);
			}

			// Build Gateways
			if (Broodwar->self()->minerals() >= 150 + reservedMinerals) {
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
			}
		}
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
	if (unit->getType().isWorker() && unit->getPlayer() == Broodwar->self()) {
		workerCount++;
	}

	if (unit->getType() == UnitTypes::Protoss_Pylon && unit->getPlayer() == Broodwar->self()) {
		reservedMinerals -= 100;
	}
	if (unit->getType() == UnitTypes::Protoss_Gateway && unit->getPlayer() == Broodwar->self()) {
		reservedMinerals -= 150;
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
	if (unit->getType().isWorker() && unit->getPlayer() == Broodwar->self()) {
		workerCount--;
	}

	
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit) {
	
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit) {
}

void ExampleAIModule::onSaveGame(std::string gameName) {
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit) {

}

bool ExampleAIModule::supplyNeeded(BWAPI::Unit u) {
	if (availableSupply <= nexusCount + gatewayCount * 2 && Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY && Broodwar->self()->minerals() >= 100 + reservedMinerals) {
		return true;
	}
	return false;
}

void ExampleAIModule::buildSupply(BWAPI::Unit u) {
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
}

bool ExampleAIModule::workerNeeded(BWAPI::Unit u) {
	if (availableSupply >= 1
	&& Broodwar->self()->minerals() >= 50 + reservedMinerals
	&& u->getType().isResourceDepot()
	&& Broodwar->self()->incompleteUnitCount(u->getType().getRace().getSupplyProvider()) > 0) {
		return true;
	}
	return false;
}

bool ExampleAIModule::canBuildWorker(BWAPI::Unit u) {
	if (workerCount < MAX_WORKERS
	&& u->isIdle()) {
		return true;
	}
	return false;
}

void ExampleAIModule::buildWorker(BWAPI::Unit u) {
	u->train(u->getType().getRace().getWorker());
}
