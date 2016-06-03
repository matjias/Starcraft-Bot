#pragma once

#include "UnitHandler.h"
#include "BuildingUnits.h"

#include <BWAPI.h>

class UnitOrUpgrade {
public:
	UnitOrUpgrade(BWAPI::UnitType unitType);
	UnitOrUpgrade(BWAPI::UpgradeType upgradeType);
	~UnitOrUpgrade();

	BWAPI::UnitType getUnitType();
	BWAPI::UpgradeType getUpgradeType();

	bool isUnitType() const;
	bool isUpgradeType() const;

	UnitOrUpgrade type();

	int gasPrice();

private:
	BWAPI::UnitType unitType;
	BWAPI::UpgradeType upgradeType;

	UnitOrUpgrade & operator=(BWAPI::UnitType u);
	UnitOrUpgrade & operator=(BWAPI::UpgradeType u);
};

class ResourceSpender {
public:
	ResourceSpender();
	~ResourceSpender();
	bool _init(UnitHandler* UnitHandler, BuildingUnits* buildingUnits);
	void setAllIn(bool buildWorkers);

private:
	void addInvestment(BWAPI::UnitType unitType, bool urgent);
	void addInvestment(BWAPI::UpgradeType unitType, bool urgent);
	void addNeededTech(int priority);
	bool investmentExists(UnitOrUpgrade investment);
	bool workerNeeded();
	bool supplyNeeded();
	int unitsInProgress(BWAPI::UnitType unitType);
	bool upgradeInProgress(BWAPI::UpgradeType upgradeType);
	int getMaxSupplyOutput();
	

	std::vector<UnitOrUpgrade> investments;
	std::vector<BWAPI::UpgradeType> upgradesInProgress;

	bool allIn;

	UnitHandler* unitHandlerPtr;
	BuildingUnits* buildingUnitsPtr;
};
