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

	BWAPI::UnitType getUnitType();
	BWAPI::UpgradeType getUpgradeType();

	bool isUnitType() const;
	bool isUpgradeType() const;

	UnitOrUpgrade type();

	int gasPrice();

	//UnitOrUpgrade & operator=(BWAPI::UpgradeType upgradeType);
	void operator= (const BWAPI::UnitType &unitType);
	void operator= (const BWAPI::UpgradeType &upgradeType);
	
	bool operator== (const BWAPI::UnitType &unitType) const;
	bool operator== (const BWAPI::UpgradeType &upgradeType) const;

	bool operator!= (const BWAPI::UnitType &unitType) const;
	bool operator!= (const BWAPI::UpgradeType &upgradeType) const;

private:
	BWAPI::UnitType unitType;
	BWAPI::UpgradeType upgradeType;
};

class ResourceSpender {
public:
	ResourceSpender();
	~ResourceSpender();
	bool _init(UnitHandler* UnitHandler, BuildingUnits* buildingUnits, ProbeUnits* probeUnits);
	void setAllIn(bool status);
	void setDefend(bool status);

private:
	void addUnitInvestment(BWAPI::UnitType unitType, bool urgent);
	void addUpgradeInvestment(BWAPI::UpgradeType unitType, bool urgent);
	void addUnitInvestment(BWAPI::UnitType unitType, int position);
	void addUpgradeInvestment(BWAPI::UpgradeType unitType, int position);
	void addRequirements(int priority);
	bool investmentExists(UnitOrUpgrade investment);
	bool unitInvestmentExists(BWAPI::UnitType investment);
	bool upgradeInvestmentExists(BWAPI::UpgradeType investment);
	bool workerNeeded();
	bool supplyNeeded();
	int unitsInProgress(BWAPI::UnitType unitType);
	bool upgradeInProgress(BWAPI::UpgradeType upgradeType);
	int getMaxSupplyOutput();
	

	std::vector<UnitOrUpgrade> investments;
	std::vector<BWAPI::UpgradeType> upgradesInProgress;

	bool allIn;
	bool defend;

	UnitHandler* unitHandlerPtr;
	BuildingUnits* buildingUnitsPtr;
	ProbeUnits* probeUnitsPtr;
};
