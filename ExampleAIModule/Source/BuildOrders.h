#pragma once
#include <BWAPI.h>

class BuildOrders
{
public:
	BuildOrders();
	~BuildOrders();

	std::vector<BWAPI::UnitType> buildOrderTest1;
	std::vector<BWAPI::UnitType> buildOrderTest2;
	std::vector<BWAPI::UnitType> buildOrderInitial;
	std::vector<BWAPI::UnitType> buildOrder2Gate;

	std::vector<BWAPI::UnitType> getBuild(std::string build);
};
