#include "BuildOrders.h"
#include "Constants.h"
#include <vector>

BuildOrders::BuildOrders() {
	nexusCount = 1;
	probeCount = 4;

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

BuildOrders::~BuildOrders() {
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
	}
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
			secondBuildOrderStarted = true;
			investmentList = buildOrder2Gate;
			//investmentList = buildOrderDragoonRush;
			updateQueueValues();
		}
	}
}

void BuildOrders::addInvestments() {
	if (pylonNeeded()) {
		investmentList.push_back(BWAPI::UnitTypes::Protoss_Pylon);
		supplyBuffer += BWAPI::UnitTypes::Protoss_Pylon.supplyProvided() / 2;
		pylonsQueued++;
	}
	if (gatewayNeeded()) {
		investmentList.push_back(BWAPI::UnitTypes::Protoss_Gateway);
		gatewaysQueued++;
	}

	if (workerNeeded()) {
		investmentList.push_back(BWAPI::UnitTypes::Protoss_Probe);
		probesQueued++;
	}
	if (zealotNeeded()) {
		investmentList.push_back(BWAPI::UnitTypes::Protoss_Zealot);
		zealotsQueued++;
	}
	if (dragoonNeeded()) {
		investmentList.push_back(BWAPI::UnitTypes::Protoss_Dragoon);
		dragoonsQueued++;
	}
}

void BuildOrders::reorderInvestments() {
	if (gatewaysQueued
		&& gatewayCount <= zealotsWarping + dragoonsWarping
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
		&& zealotsWarping + dragoonsWarping < gatewayCount
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
		&& zealotsWarping + dragoonsWarping < gatewayCount
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
		&& probesWarping < nexusCount
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
		&& availableSupply <= nexusCount + gatewayCount * 2
		&& BWAPI::Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY
		&& investmentList[0] != BWAPI::UnitTypes::Protoss_Pylon) {
		for (int i = 0; i < investmentList.size(); i++) {
			if (investmentList[i] == BWAPI::UnitTypes::Protoss_Pylon) {
				investmentList.erase(investmentList.begin() + i);
				investmentList.insert(investmentList.begin(), BWAPI::UnitTypes::Protoss_Pylon);
				break;
			}
		}
	}
}

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
	return (!pylonCount
		&& !pylonsWarping
		&& !pylonsQueued
		|| availableSupply + supplyBuffer <= (nexusCount + nexusesWarping) * 1 + (gatewayCount + gatewaysWarping) * 2
		&& BWAPI::Broodwar->self()->supplyTotal() / 2 < MAX_SUPPLY);
}

bool BuildOrders::gatewayNeeded() {
	return !gatewaysQueued && gatewayCount <= zealotsWarping + dragoonsWarping
		&& BWAPI::Broodwar->self()->minerals() - reservedMinerals > gatewayCount * BWAPI::UnitTypes::Protoss_Zealot.mineralPrice();
}

bool BuildOrders::workerNeeded() {
	return !probesQueued && probeCount + probesQueued < MAX_WORKERS;
}

bool BuildOrders::zealotNeeded() {
	return !zealotsQueued && (!cyberneticsCoreCount || (BWAPI::Broodwar->self()->minerals() - reservedMinerals > MINERAL_SURPLUS_LIMIT && !BWAPI::Broodwar->self()->gas() - reservedGas > GAS_SURPLUS_LIMIT));
}

bool BuildOrders::dragoonNeeded() {
	return !dragoonsQueued;
}
