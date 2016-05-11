#pragma once
#include <BWAPI.h>


class ProbeUnits {
public:
	ProbeUnits();
	~ProbeUnits();

	void mineMinerals(BWAPI::Unitset uSet);
	void mineMinerals(BWAPI::Unit u);
	void mineGas(BWAPI::Unitset uSet);


private:
	int miningCount;

	
};
