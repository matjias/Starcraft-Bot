#pragma once
#include <BWAPI.h>

class BuildOrders
{
public:
	BuildOrders();
	~BuildOrders();

	void useInitialBuildOrder();
	void useSecondBuildOrder();
	void addInvestments();
	void reorderInvestments();

	std::vector<BWAPI::UnitType> buildOrder;
	std::vector<BWAPI::UnitType> investmentList;

	std::vector<BWAPI::UnitType> buildOrderInitial;
	std::vector<BWAPI::UnitType> buildOrder2Gate;

	std::vector<BWAPI::UnitType> getInvestmentList();
	void dequeueInvestmentList();
	bool getFirstBuildOrderFinished();
	bool getSecondBuildOrderFinished();
	int getAvailableSupply();
	void setAvailableSupply(int a);
	int getSupplyBuffer();
	void setSupplyBuffer(int a);
	int getReservedMinerals();
	void setReservedMinerals(int a);
	int getReservedGas();
	void setReservedGas(int a);

	// Buildings
	int nexusCount;
	int nexusesWarping;
	int nexusesQueued;
	int pylonCount;
	int pylonsWarping;
	int pylonsQueued;
	int gatewayCount;
	int gatewaysWarping;
	int gatewaysQueued;

	// Units
	int probeCount;
	int probesQueued;
	int probesWarping;
	int zealotCount;
	int zealotsWarping;
	int zealotsQueued;

private:
	void updateQueueValues();

	bool pylonNeeded();
	bool gatewayNeeded();
	bool workerNeeded();
	bool zealotNeeded();

	bool firstBuildOrderStarted;
	bool firstBuildOrderFinished;
	bool secondBuildOrderStarted;
	bool secondBuildOrderFinished;

	int availableSupply;
	int supplyBuffer;
	int reservedMinerals;
	int reservedGas;
};
