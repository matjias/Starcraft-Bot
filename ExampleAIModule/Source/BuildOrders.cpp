#include "BuildOrders.h"
#include "Constants.h"
#include "ExampleAIModule.h"
#include <vector>

BuildOrders::BuildOrders() {

}

BuildOrders::~BuildOrders() {

}

void BuildOrders::_init(ExampleAIModule* m) {
	mainProgram = m;

	nexuses = 1;
	probes = 4;

	zealotRate = 1.0;
	dragoonRate = 1.0;
}

void BuildOrders::useInitialBuildOrder() {
	if (investmentList.empty()) {
		if (firstBuildOrderStarted) {
			firstBuildOrderFinished = true;
		}
		else {
			firstBuildOrderStarted = true;
			investmentList = buildOrderInitial;
			updateQueueValues();
		}
	}
}

void BuildOrders::useSecondBuildOrder() {
	if (investmentList.empty()) {
		if (secondBuildOrderStarted) {
			secondBuildOrderFinished = true;
		}
		else {

			// VS Protoss
			if (true /*enemyRace == Protoss*/) {
				if (false /*closeToEnemyBase*/) {
					investmentList = buildOrder2Gate;
					cutProbes = true;
				}
				else if (true /*closeToEnemyBase*/) {
					investmentList = buildOrderDragoonRush;
				}
			}

			// VS Terran
			else if (false /*enemyRace == Terran*/) {
				if (false /*closeToEnemyBase*/) {
					investmentList = buildOrder2Gate;
				}
				else if (false /*closeToEnemyBase*/) {
					investmentList = buildOrderDragoonRush;
				}
			}

			// VS Zerg
			else if (false /*enemyRace == Zerg*/) {
				if (false /*closeToEnemyBase*/) {
					investmentList = buildOrder2Gate;
				}
				else if (true /*closeToEnemyBase*/) {
					investmentList = buildOrderDragoonRush;
				}
			}

			// VS undiscovered random race
			else {
				if (false /*closeToEnemyBase*/) {
					investmentList = buildOrder2Gate;
				}
				else if (false /*closeToEnemyBase*/) {
					investmentList = buildOrderDragoonRush;
				}
			}

			secondBuildOrderStarted = true;
			investmentList = buildOrder2Gate;
			//investmentList = buildOrderDragoonRush;
			updateQueueValues();
		}
	}
}

void BuildOrders::addInvestments() {
	if (pylonNeeded()) {
		investInPylon();
	}
	if (gatewayNeeded()) {
		investInGateway();
	}

	if (probeNeeded()) {
		investInProbe();
	}
	if (zealotNeeded()) {
		investInZealot();
	}
	if (dragoonNeeded()) {
		investInDragoon();
	}
	if (observerNeeded()) {
		investInObserver();
	}
	if (detectionNeeded()) {
		investInObserver();
		investInPhotonCannon();
	}
	if (corsairNeeded()) {
		investInCorsair();
	}
}

void BuildOrders::addRequiredTech() {

	// Add required Stargate
	if (!stargates && investmentList[0] == BWAPI::UnitTypes::Protoss_Corsair) {
		if (stargatesWarping) {
			moveInvestmentDown(investmentList[0]);
		}
		else {
			if (!stargatesQueued) {
				investInStargate();
			}
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Stargate);
		}
	}

	// Add required Observatory
	if (!observatories && (investmentList[0] == BWAPI::UnitTypes::Protoss_Observer)) {
		if (observatoriesWarping) {
			moveInvestmentDown(investmentList[0]);
		}
		else {
			if (!observatoriesQueued) {
				investInObservatory();
			}
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Observatory);
		}
	}

	// Add required Robotics Facility
	if (!roboticsFacilities && (investmentList[0] == BWAPI::UnitTypes::Protoss_Observer || investmentList[0] == BWAPI::UnitTypes::Protoss_Observatory)) {
		if (roboticsFacilitiesWarping) {
			moveInvestmentDown(investmentList[0]);
		}
		else {
			if (!roboticsFacilitiesQueued) {
				investInRoboticsFacility();
			}
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Robotics_Facility);
		}
	}

	// Add required Cybernetics Core
	if (!cyberneticsCores && (investmentList[0] == BWAPI::UnitTypes::Protoss_Dragoon
		|| investmentList[0] == BWAPI::UnitTypes::Protoss_Robotics_Facility
		|| investmentList[0] == BWAPI::UnitTypes::Protoss_Stargate)) {
		if (cyberneticsCoresWarping) {
			moveInvestmentDown(investmentList[0]);
		}
		else {
			if (!cyberneticsCoresQueued) {
				investInCyberneticsCore();
			}
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
		}
	}

	// Add required Assimilator
	if ((assimilators + assimilatorsWarping + assimilatorsQueued < nexuses) && (cyberneticsCores || cyberneticsCoresWarping || cyberneticsCoresQueued)) {
		if (!assimilatorsQueued) {
			investInAssimilator();
		}
		moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Assimilator);
	}

	// Add required Forge
	if (!forges && investmentList[0] == BWAPI::UnitTypes::Protoss_Photon_Cannon) {
		if (forgesWarping) {
			moveInvestmentDown(investmentList[0]);
		}
		else {
			if (!forgesQueued) {
				investInForge();
			}
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Forge);
		}
	}

	// Add required Gateway
	if (!gateways && (investmentList[0] == BWAPI::UnitTypes::Protoss_Zealot
		|| investmentList[0] == BWAPI::UnitTypes::Protoss_Dragoon
		|| investmentList[0] == BWAPI::UnitTypes::Protoss_Cybernetics_Core)) {
		if (gatewaysWarping) {
			moveInvestmentDown(investmentList[0]);
		}
		else {
			if (!gatewaysQueued) {
				investInGateway();
			}
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Gateway);
		}
	}

	// Add required Pylon
	if (!pylons && investmentList[0] == BWAPI::UnitTypes::Protoss_Gateway) {
		if (pylonsWarping) {
			moveInvestmentDown(investmentList[0]);
		}
		else {
			if (!pylonsQueued) {
				investInPylon();
			}
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Pylon);
		}
	}
}

void BuildOrders::reorderInvestments() {

	int prioritiesMovedDown = 0;

	// Spend surplus resources
	if (BWAPI::Broodwar->self()->minerals() - reservedMinerals > investmentList[0].mineralPrice()
		&& BWAPI::Broodwar->self()->gas() - reservedGas > investmentList[0].gasPrice()) {
		
		/*if (roboticsFacilities > observersWarping && observersQueued && observatories
			&& canAffortSurplusUnit(BWAPI::UnitTypes::Protoss_Observer)) {
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Observer);
		}

		else if (stargates > corsairsWarping && corsairsQueued
			&& canAffortSurplusUnit(BWAPI::UnitTypes::Protoss_Corsair)) {
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Corsair);
		}

		else*/ if (gateways > zealotsWarping + dragoonsWarping && dragoonsQueued && cyberneticsCores
			&& canAffortSurplusUnit(BWAPI::UnitTypes::Protoss_Dragoon)) {
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Dragoon);
		}

		else if (gateways > zealotsWarping + dragoonsWarping && zealotsQueued
			&& canAffortSurplusUnit(BWAPI::UnitTypes::Protoss_Zealot)) {
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Zealot);
		}
	}

	// Invest in additional production facilities
	if (gateways <= zealotsWarping + dragoonsWarping) {
		if (investmentList[0] != BWAPI::UnitTypes::Protoss_Gateway
			&& BWAPI::Broodwar->self()->minerals() - reservedMinerals > (gateways + gatewaysWarping) * BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice() + investmentList[0].mineralPrice()) {
			moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Gateway);
		}
	}
	else if (investmentList[0] == BWAPI::UnitTypes::Protoss_Gateway) {
		moveInvestmentDown(investmentList[0]);
	}

	// Probe production
	if (probesWarping < nexuses && probes + probesWarping + probesQueued < MAX_WORKERS && !cutProbes) {
		moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Probe);
	}
	else if (investmentList[0] == BWAPI::UnitTypes::Protoss_Probe) {
		moveInvestmentDown(investmentList[0]);
	}

	// Detection
	/*if (detectionNeeded() && investmentList[0] != BWAPI::UnitTypes::Protoss_Photon_Cannon && investmentList[0] != BWAPI::UnitTypes::Protoss_Observer && (photonCannonsQueued || observersQueued)) {
	if (photonCannonsQueued && (float) ((photonCannons + photonCannonsWarping) / (PHOTON_CANNONS_PER_NEXUS * nexuses)) < (float)((observers + observersWarping) / OBSERVERS_NEEDED)) {
	moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Photon_Cannon);
	}
	else if (observersQueued) {
	moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Observer);
	}
	}*/

	// Supply
	if (availableSupply <= getProductionSupply() && investmentList[0] != BWAPI::UnitTypes::Protoss_Pylon) {
		moveInvestmentToTop(BWAPI::UnitTypes::Protoss_Pylon);
	}



	/*
	if (investmentList[0] == BWAPI::UnitTypes::Protoss_Dragoon
		&& !(cyberneticsCores + cyberneticsCoresWarping + cyberneticsCoresQueued)) {
		investInCyberneticsCore();
	}
	if (investmentList[0] == BWAPI::UnitTypes::Protoss_Dragoon
		&& !(assimilators + assimilatorsWarping + assimilatorsQueued)) {
		investInAssimilator();
	}
	if ((investmentList[0] == BWAPI::UnitTypes::Protoss_Zealot
		|| investmentList[0] == BWAPI::UnitTypes::Protoss_Dragoon
		|| investmentList[0] == BWAPI::UnitTypes::Protoss_High_Templar
		|| investmentList[0] == BWAPI::UnitTypes::Protoss_Dark_Templar) && !(gateways + gatewaysWarping + gatewaysQueued)) {
		investInGateway();
	}
	if (investmentList[0] == BWAPI::UnitTypes::Protoss_Gateway
		&& !(pylons + pylonsWarping + pylonsQueued)) {
		investInPylon();
	}

	if (gatewaysQueued
		&& gateways <= zealotsWarping + dragoonsWarping
		&& investmentList[0] == BWAPI::UnitTypes::Protoss_Zealot) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == BWAPI::UnitTypes::Protoss_Gateway) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), BWAPI::UnitTypes::Protoss_Gateway);
				break;
			}
		}
	}
	if (zealotsQueued
		&& zealotsWarping + dragoonsWarping < gateways
		&& BWAPI::Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY
		&& investmentList[0] != BWAPI::UnitTypes::Protoss_Zealot) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == BWAPI::UnitTypes::Protoss_Zealot) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), BWAPI::UnitTypes::Protoss_Zealot);
				break;
			}
		}
	}
	if (dragoonsQueued
		&& zealotsWarping + dragoonsWarping < gateways
		&& BWAPI::Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY
		&& investmentList[0] != BWAPI::UnitTypes::Protoss_Dragoon) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == BWAPI::UnitTypes::Protoss_Dragoon) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), BWAPI::UnitTypes::Protoss_Dragoon);
				break;
			}
		}
	}
	if (probesQueued
		&& probesWarping < nexuses
		&& BWAPI::Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY
		&& investmentList[0] != BWAPI::UnitTypes::Protoss_Probe) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == BWAPI::UnitTypes::Protoss_Probe) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), BWAPI::UnitTypes::Protoss_Probe);
				break;
			}
		}
	}
	if (pylonsQueued
		&& availableSupply <= nexuses + gateways * 2
		&& BWAPI::Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY
		&& investmentList[0] != BWAPI::UnitTypes::Protoss_Pylon) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == BWAPI::UnitTypes::Protoss_Pylon) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), BWAPI::UnitTypes::Protoss_Pylon);
				break;
			}
		}
	}*/
}

bool BuildOrders::canAffortSurplusUnit(BWAPI::UnitType u) {
	return BWAPI::Broodwar->self()->minerals() - reservedMinerals >= investmentList[0].mineralPrice() + u.mineralPrice()
		&& BWAPI::Broodwar->self()->gas() - reservedGas >= investmentList[0].gasPrice() + u.gasPrice()
		&& availableSupply >= investmentList[0].supplyRequired() + u.supplyRequired();
}

void BuildOrders::moveInvestmentToTop(BWAPI::UnitType t) {
	for (int i = 1; i < investmentList.size(); i++) {
		if (investmentList[i] == t) {
			investmentList.erase(investmentList.begin() + i);
			investmentList.insert(investmentList.begin(), t);
			break;
		}
	}
}

void BuildOrders::moveInvestmentDown(BWAPI::UnitType t) {
	for (int i = 0; i < investmentList.size() - 1; i++) {
		if (investmentList[i] == t) {
			investmentList.erase(investmentList.begin() + i);

			if (investmentList.size() > i + prioritiesMovedDown + 1) {
				investmentList.insert(investmentList.begin() + i + prioritiesMovedDown + 1, t);
			}
			else {
				investmentList.push_back(t);
			}
			prioritiesMovedDown++;
			break;
		}
	}
}

/*void BuildOrders::moveLastInvestmentToTop() {
	investmentList.insert(investmentList.begin(), investmentList[investmentList.size() - 1]);
	investmentList.pop_back();
}*/

std::vector<BWAPI::UnitType> BuildOrders::getInvestmentList() {
	return investmentList;
}

void BuildOrders::dequeueInvestmentList() {
	investmentList.erase(investmentList.begin());
}

bool BuildOrders::getFirstBuildOrderFinished() {
	return firstBuildOrderFinished;
}

bool BuildOrders::getSecondBuildOrderFinished() {
	return secondBuildOrderFinished;
}

int BuildOrders::getAvailableSupply() {
	return availableSupply;
}

void BuildOrders::setAvailableSupply(int a) {
	availableSupply = a;
}

int BuildOrders::getSupplyBuffer() {
	return supplyBuffer;
}

void BuildOrders::setSupplyBuffer(int a) {
	supplyBuffer = a;
}


int BuildOrders::getReservedMinerals() {
	return reservedMinerals;
}

void BuildOrders::setReservedMinerals(int a) {
	reservedMinerals = a;
}

int BuildOrders::getReservedGas() {
	return reservedGas;
}

void BuildOrders::setReservedGas(int a) {
	reservedGas = a;
}

bool BuildOrders::pylonNeeded() {
	return (availableSupply + supplyBuffer <= getProductionSupply()
		&& BWAPI::Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY);
}

int BuildOrders::getProductionSupply() {
	return ((nexuses + nexusesWarping) * BWAPI::UnitTypes::Protoss_Probe.supplyRequired() +
		(gateways + gatewaysWarping) * BWAPI::UnitTypes::Protoss_Zealot.supplyRequired() +
		(roboticsFacilities + roboticsFacilitiesWarping) * BWAPI::UnitTypes::Protoss_Reaver.supplyRequired() +
		(stargates + stargatesWarping) * BWAPI::UnitTypes::Protoss_Carrier.supplyRequired()) / 2;
}

bool BuildOrders::gatewayNeeded() {
	return !gatewaysQueued && gateways <= zealotsWarping + dragoonsWarping
		&& BWAPI::Broodwar->self()->minerals() - reservedMinerals > (gateways + gatewaysWarping) * BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice();
}

bool BuildOrders::probeNeeded() {
	return !probesQueued && probesWarping < nexuses && probes + probesWarping + probesQueued < MAX_WORKERS;
}

bool BuildOrders::zealotNeeded() {
	return !zealotsQueued
		&& zealotRate >= 0
		&& (zealots + zealotsWarping + zealotsQueued) * zealotRate <= (dragoons + dragoonsWarping + dragoonsQueued) * dragoonRate;
		//&& ((BWAPI::Broodwar->self()->minerals() - reservedMinerals > MINERAL_SURPLUS_LIMIT && !BWAPI::Broodwar->self()->gas() - reservedGas > GAS_SURPLUS_LIMIT));
}

bool BuildOrders::dragoonNeeded() {
	return !dragoonsQueued
		&& dragoonRate >= 0
		&& (dragoons + dragoonsWarping + dragoonsQueued) * dragoonRate <= (zealots + zealotsWarping + zealotsQueued) * zealotRate;
}

bool BuildOrders::observerNeeded() {
	return !observersQueued
		&& (cyberneticsCores && assimilators || roboticsFacilities || observatories)
		&& (observers + observersWarping + observersQueued < MIN_OBSERVERS
		|| (observers + observersWarping + observersQueued < OBSERVERS_TO_DETECT && invisibleSpotted));
}

bool BuildOrders::detectionNeeded() {
	return false; // (invisibleSpotted || enemyBuildings.contains(BWAPI::UnitTypes::Protoss_Templars_Archives) || enemyBuildings.contains(BWAPI::UnitTypes::Terran_Factory) || enemyBuildings.contains(BWAPI::UnitTypes::Terran_Starport) || enemyBuildings.contains(BWAPI::UnitTypes::Terran_Control_Tower) || enemyBuildings.contains(BWAPI::UnitTypes::Terran_Science_Facility) || enemyBuildings.contains(BWAPI::UnitTypes::Terran_Control_Tower) || enemyBuildings.contains(BWAPI::UnitTypes::Zerg_Hydralisk_Den) || enemyBuildings.contains(BWAPI::UnitTypes::Zerg_Lair))
	//&& ((observers + observersWarping + observersQueued) < OBSERVERS_NEEDED || (photonCannons + photonCannonsWarping + photonCannonsQueued) < CANNONS_PER_NEXUS * nexuses;
}

bool BuildOrders::corsairNeeded() {
	return !corsairsQueued
		&& ((detectionNeeded()
		&& observers >= OBSERVERS_TO_DETECT
		/*&& mainProgram->getEnemyStructures().find(BWAPI::UnitTypes::Zerg_Spore_Colony) != mainProgram->getEnemyStructures().end()*/)
		|| stargates + stargatesWarping)
		&& corsairs + corsairsWarping + corsairsQueued < CORSAIRS_NEEDED
		&& BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg;
}

void BuildOrders::updateQueueValues() {
	for (int i = 0; i < investmentList.size(); i++) {

		if (investmentList[i] == BWAPI::UnitTypes::Protoss_Nexus) {
			nexusesQueued++;
			supplyBuffer += BWAPI::UnitTypes::Protoss_Nexus.supplyProvided() / 2;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Pylon) {
			pylonsQueued++;
			supplyBuffer += BWAPI::UnitTypes::Protoss_Pylon.supplyProvided() / 2;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Gateway) {
			gatewaysQueued++;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Assimilator) {
			assimilatorsQueued++;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Cybernetics_Core) {
			cyberneticsCoresQueued++;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Probe) {
			probesQueued++;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Zealot) {
			zealotsQueued++;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Dragoon) {
			dragoonsQueued++;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Observer) {
			observersQueued++;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Corsair) {
			corsairsQueued++;
		}
	}
}

void BuildOrders::investInPylon() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Pylon);
	supplyBuffer += BWAPI::UnitTypes::Protoss_Pylon.supplyProvided() / 2;
	pylonsQueued++;
}

void BuildOrders::investInGateway() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Gateway);
	gatewaysQueued++;
}

void BuildOrders::investInForge() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Forge);
	forgesQueued++;
}

void BuildOrders::investInPhotonCannon() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Photon_Cannon);
	photonCannonsQueued++;
}

void BuildOrders::investInAssimilator() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Assimilator);
	assimilatorsQueued++;
}

void BuildOrders::investInCyberneticsCore() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
	cyberneticsCoresQueued++;
}

void BuildOrders::investInRoboticsFacility () {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Robotics_Facility);
	roboticsFacilitiesQueued++;
}

void BuildOrders::investInObservatory() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Observatory);
	observatoriesQueued++;
}

void BuildOrders::investInStargate() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Stargate);
	stargatesQueued++;
}

void BuildOrders::investInProbe() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Probe);
	probesQueued++;
}

void BuildOrders::investInZealot() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Zealot);
	zealotsQueued++;
}

void BuildOrders::investInDragoon() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Dragoon);
	dragoonsQueued++;
}

void BuildOrders::investInObserver() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Observer);
	observersQueued++;
}

void BuildOrders::investInCorsair() {
	investmentList.push_back(BWAPI::UnitTypes::Protoss_Corsair);
	corsairsQueued++;
}

void BuildOrders::defineBuildOrders() {

	// Initial build order
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Pylon);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);

	// 2 Gate - "9/9 Gateways"
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Gateway);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Gateway);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Zealot);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Pylon);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Zealot);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Zealot);

	// Dragoon Rush - "One Zealot before Cybernetics Core"
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Gateway);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Assimilator);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Zealot);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Pylon);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderDragoonRush.push_back(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
}
