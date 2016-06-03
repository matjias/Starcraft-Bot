#pragma once
#include <BWAPI.h>

class ResourceSpender {
public:
	ResourceSpender();
	~ResourceSpender();

private:
	void addInvestment(BWAPI::UnitType unitType, bool urgent);
	void addNeededTech(int priority);
	void workerNeeded();
	void supplyNeeded();
};
