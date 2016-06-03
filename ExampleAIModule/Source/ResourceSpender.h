#pragma once

#include "UnitHandler.h"

#include <BWAPI.h>

class UnitOrUpgrade {
public:
	UnitOrUpgrade(BWAPI::UnitType unitType);
	UnitOrUpgrade(BWAPI::UpgradeType upgradeType);
	~UnitOrUpgrade();

private:
	BWAPI::UnitType unitType;
	BWAPI::UpgradeType upgradeType;

	BWAPI::UnitType getUnitType();
	BWAPI::UpgradeType getUpgradeType();

	bool isUnitType() const;
	bool isUpgradeType() const;

	UnitOrUpgrade & operator=(BWAPI::UnitType u);
	UnitOrUpgrade & operator=(BWAPI::UpgradeType u);
};

class ResourceSpender {
public:
	ResourceSpender();
	~ResourceSpender();
	bool _init(UnitHandler* unitHandler);

private:
	void addInvestment(BWAPI::UnitType unitType, bool urgent);
	void addInvestment(BWAPI::UpgradeType unitType, bool urgent);
	void addNeededTech(int priority);
	bool workerNeeded();
	bool supplyNeeded();
	int getMaxSupplyOutput();

	std::vector<UnitOrUpgrade> investments;

	UnitHandler* unitHandlerPtr;
};
