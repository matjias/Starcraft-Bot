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

	int gasPrice();

	bool operator== (const UnitOrUpgrade &investment) const;
	bool operator!= (const UnitOrUpgrade &investment) const;

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
	bool investmentExists(UnitOrUpgrade investment);
	bool unitInvestmentExists(BWAPI::UnitType investment);
	void addUnitInvestment(BWAPI::UnitType investment, bool urgent);
	void addUpgradeInvestment(BWAPI::UpgradeType investment, bool urgent);

private:
	void addUnitInvestment(BWAPI::UnitType investment, int position);
	void addUpgradeInvestment(BWAPI::UpgradeType investment, int position);
	void addRequirements(int priority);

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
