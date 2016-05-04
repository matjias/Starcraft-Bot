#pragma once
#include <BWAPI.h>
#include "ExampleAIModule.h"
#include "Scouting.h"

class BuildOrders
{
public:
	BuildOrders();
	~BuildOrders();

	void _init(Scouting* scoutClass);

	void useInitialBuildOrder();
	void useSecondBuildOrder();
	void addInvestments();
	void addRequiredTech();
	void reorderInvestments();

	std::vector<BWAPI::UnitType> buildOrder;
	std::vector<BWAPI::UnitType> investmentList;

	std::vector<BWAPI::UnitType> buildOrderInitial;
	std::vector<BWAPI::UnitType> buildOrder2Gate;
	std::vector<BWAPI::UnitType> buildOrderDragoonRush;

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
	bool getAllIn();

	int getProductionSupply();

	// Buildings
	int nexuses;
	int nexusesWarping;
	int nexusesQueued;
	int pylons;
	int pylonsWarping;
	int pylonsQueued;
	int gateways;
	int gatewaysWarping;
	int gatewaysQueued;
	int forges;
	int forgesWarping;
	int forgesQueued;
	int photonCannons;
	int photonCannonsWarping;
	int photonCannonsQueued;
	int assimilators;
	int assimilatorsWarping;
	int assimilatorsQueued;
	int cyberneticsCores;
	int cyberneticsCoresWarping;
	int cyberneticsCoresQueued;
	int roboticsFacilities;
	int roboticsFacilitiesWarping;
	int roboticsFacilitiesQueued;
	int observatories;
	int observatoriesWarping;
	int observatoriesQueued;
	int stargates;
	int stargatesWarping;
	int stargatesQueued;

	// Units
	int probes;
	int probesQueued;
	int probesWarping;
	int zealots;
	int zealotsWarping;
	int zealotsQueued;
	int dragoons;
	int dragoonsWarping;
	int dragoonsQueued;
	int observers;
	int observersWarping;
	int observersQueued;
	int corsairs;
	int corsairsWarping;
	int corsairsQueued;

	float zealotRate; // TODO: Move to private
	float dragoonRate;

private:
	// Initial functions
	void defineBuildOrders();
	void updateQueueValues();

	bool firstBuildOrderStarted;
	bool firstBuildOrderFinished;
	bool secondBuildOrderStarted;
	bool secondBuildOrderFinished;
	
	// Investment functions
	void moveInvestmentToTop(BWAPI::UnitType u);
	void moveInvestmentDown(BWAPI::UnitType u);
	bool canAffortSurplusUnit(BWAPI::UnitType u);

	void investInPylon();
	void investInGateway();
	void investInForge();
	void investInPhotonCannon();
	void investInAssimilator();
	void investInCyberneticsCore();
	void investInRoboticsFacility();
	void investInObservatory();
	void investInStargate();

	void investInProbe();
	void investInZealot();
	void investInDragoon();
	void investInObserver();
	void investInCorsair();

	/*void investInGroundWeapons();
	void investInGroundArmor();
	void investInSingularityCharge();*/

	bool detectionNeeded();
	bool pylonNeeded();
	bool gatewayNeeded();
	bool probeNeeded();
	bool zealotNeeded();
	bool dragoonNeeded();
	bool observerNeeded();
	bool corsairNeeded();

	/*bool groundWeaponsNeeded();
	bool groundArmorNeeded();
	bool singularityChargeNeeded();*/

	// Map and enemy information functions
	bool enemyBaseFound();
	bool smallMap();
	bool closeToEnemyBase();

	// Variables
	Scouting* scoutClass;

	int availableSupply;
	int supplyBuffer;
	int reservedMinerals;
	int reservedGas;

	bool invisibleSpotted;

	int prioritiesMovedDown;

	//float zealotRate; TODO: Uncomment
	//float dragoonRate;
	bool allIn;

};
