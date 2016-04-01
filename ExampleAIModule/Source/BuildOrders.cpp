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

	// 2 gate
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Gateway);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Gateway);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Zealot);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Pylon);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Zealot);
	buildOrder2Gate.push_back(BWAPI::UnitTypes::Protoss_Zealot);
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
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Probe) {
			probesQueued++;
		}
		else if (investmentList[i] == BWAPI::UnitTypes::Protoss_Zealot) {
			zealotsQueued++;
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
}

void BuildOrders::reorderInvestments() {
	if (gatewaysQueued
		&& gatewayCount <= zealotsWarping
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
		&& zealotsWarping < gatewayCount
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
	return !gatewaysQueued && (gatewayCount + gatewaysQueued < nexusCount * 4 || BWAPI::Broodwar->self()->minerals() < MINERAL_SURPLUS_LIMIT + reservedMinerals);
}

bool BuildOrders::workerNeeded() {
	return !probesQueued && probeCount + probesQueued < MAX_WORKERS;
}

bool BuildOrders::zealotNeeded() {
	return !zealotsQueued;
}