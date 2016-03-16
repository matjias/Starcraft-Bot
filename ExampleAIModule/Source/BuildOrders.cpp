#include "BuildOrders.h"
#include <vector>

BuildOrders::BuildOrders() {

	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Pylon);
	buildOrderInitial.push_back(BWAPI::UnitTypes::Protoss_Probe);

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
