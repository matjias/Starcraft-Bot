#pragma once

#include "UnitHandler.h"
#include "BuildingUnits.h"
#include "Constants.h"
#include <BWAPI.h>

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
	void setAllIn(bool status);
	void setDefend(bool status);
	bool unitInvestmentExists(BWAPI::UnitType investment);
	void addUnitInvestment(BWAPI::UnitType investment, bool urgent);
	void addUpgradeInvestment(BWAPI::UpgradeType investment, bool urgent);

private:
	void addUnitInvestment(BWAPI::UnitType investment, int position);
	void addUpgradeInvestment(BWAPI::UpgradeType investment, int position);
	void addRequirements(int priority);
	void purchase();
	void calculateReservedResources();
	bool investmentExists(BWAPI::UnitType unitType);
	bool investmentExists(BWAPI::UpgradeType upgradeType);
	bool canBuildUnit(BWAPI::UnitType unitType);
	bool canUpgrade(BWAPI::UpgradeType upgradeType);
	bool workerNeeded();
	bool supplyNeeded();
	int unitsInProgress(BWAPI::UnitType unitType);
	bool upgradeInProgress(BWAPI::UpgradeType upgradeType);
	int getMaxSupplyOutput();

	std::vector<UnitOrUpgrade> investments;
	std::vector<BWAPI::UpgradeType> upgradesInProgress;

	bool allIn;
	bool defend;
	int reservedMinerals = 0;
	int reservedGas = 0;

	UnitHandler* unitHandlerPtr;
	BuildingUnits* buildingUnitsPtr;
	ProbeUnits* probeUnitsPtr;
};
