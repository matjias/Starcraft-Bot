#pragma once

#include "UnitHandler.h"
#include "BuildingUnits.h"
#include "Constants.h"
#include "BWAPI.h"

class UnitOrUpgrade {
public:
	UnitOrUpgrade(BWAPI::UnitType unitType);
	UnitOrUpgrade(BWAPI::UpgradeType upgradeType);
	~UnitOrUpgrade();

	BWAPI::UnitType getUnitType() const;
	BWAPI::UpgradeType getUpgradeType() const;

	bool isUnitType() const;
	bool isUpgradeType() const;

	UnitOrUpgrade type();

	int mineralPrice();
	int gasPrice();

	bool operator== (const UnitOrUpgrade &investment) const;
	bool operator!= (const UnitOrUpgrade &investment) const;

	bool pending = false;
private:
	BWAPI::UnitType unitType;
	BWAPI::UpgradeType upgradeType;
};

class ResourceSpender {
public:
	ResourceSpender();
	~ResourceSpender();
	bool _init(UnitHandler* UnitHandler, BuildingUnits* buildingUnits, ProbeUnits* probeUnits);
	void update();
	void setStrategy(StrategyName strategy);
	bool unitInvestmentExists(BWAPI::UnitType investment);
	void addUnitInvestment(BWAPI::UnitType investment, bool urgent);
	void addUpgradeInvestment(BWAPI::UpgradeType investment, bool urgent);
	BWAPI::UnitType getPendingBuilding();
	void pendingBuildingPlaced();
	void removePlacedBuildings(BWAPI::UnitType buildingType);
	BWAPI::UnitType getUnitWithPendingTech();
	bool canBuildBuilding(BWAPI::UnitType buildingType);
	bool canBuildUnit(BWAPI::UnitType unitType);
	bool canUpgrade(BWAPI::UpgradeType upgradeType);
	void setAlternateUnit(BWAPI::UnitType alternateUnit);
	void setDefaultUnit(BWAPI::UnitType alternateUnit);
private:
	void addUnitInvestment(BWAPI::UnitType investment, int position);
	void addUpgradeInvestment(BWAPI::UpgradeType investment, int position);
	void removeInvestment(int position);
	bool removeInvestments(BWAPI::UnitType investment);
	bool removeInvestments(BWAPI::UpgradeType investment);
	void removeAllDublicates();
	bool removeDublicates(int number);
	void addAllRequirements();
	bool addProductionFacilities();
	void addRequirements(int priority);
	void setPendingInvestments();
	void purchase();
	void calculateReservedResources();
	void clearReservedResources();
	bool investmentExists(BWAPI::UnitType unitType);
	bool investmentExists(BWAPI::UpgradeType upgradeType);
	bool workerNeeded();
	bool supplyNeeded();
	int unitsInProgress(BWAPI::UnitType unitType);
	bool upgradeInProgress(BWAPI::UpgradeType upgradeType);
	int getMaxSupplyOutput();
	bool canAffordUnit(BWAPI::UnitType unitType);

	std::vector<UnitOrUpgrade> investments;
	std::vector<BWAPI::UpgradeType> upgradesInProgress;
	
	int reservedMinerals = 0;
	int reservedGas = 0;
	int pendingBuilding = NULL;
	bool investmentAdded = false;
	BWAPI::UnitType unitWithPendingTech = NULL;
	BWAPI::UnitType alternateUnit = NULL;
	BWAPI::UnitType defaultUnit = NULL;
	StrategyName strategy = Default;

	UnitHandler* unitHandlerPtr;
	BuildingUnits* buildingUnitsPtr;
	ProbeUnits* probeUnitsPtr;
};
