#include "ExampleAIModule.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

int availableSupply;
int workerCount;

void ExampleAIModule::onStart() {
	Broodwar->enableFlag(Flag::UserInput);
	Broodwar->setCommandOptimizationLevel(2);


}

void ExampleAIModule::onEnd(bool isWinner) {
  
}

void ExampleAIModule::onFrame() {
	availableSupply = (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed()) / 2;

	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	//Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	Broodwar->drawTextScreen(200, 20, "Available Supply: %d", availableSupply);
	//Broodwar->drawTextScreen(200, 60, "Worker Count: %d", workerCount);
	
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
			// Gotta mine them bitcoins
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
			if (u->isIdle() && Broodwar->self()->minerals() >= 50 && 
				(availableSupply > 1 || Broodwar->self()->incompleteUnitCount(u->getType().getRace().getSupplyProvider()) > 0)) {
				u->train(u->getType().getRace().getWorker());
			}

			// Supply is getting low.. Might need another Pylon now
			if (availableSupply <= 1 && Broodwar->self()->minerals() >= 100) {
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

							// Box to display where we make the pylon
							Broodwar->registerEvent([buildLocation, pylon](Game*) {
								Broodwar->drawBoxMap(Position(buildLocation),
								Position(buildLocation + pylon.tileSize()),
								Colors::Blue);
								}, nullptr,
								pylon.buildTime() + 100);

							// Making the pylon
							supplyBuilder->build(pylon, buildLocation);
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
