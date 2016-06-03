#pragma once
#include <BWAPI.h>


class ProbeUnits {
public:
	ProbeUnits();
	~ProbeUnits();

	void addUnit(BWAPI::Unit u);

private:
	int miningCount;

	// Currently functioning as default unitset for probes.
	BWAPI::Unitset miningProbes;

	void mineMinerals(BWAPI::Unitset uSet);
	void mineMinerals(BWAPI::Unit u);
	void mineGas(BWAPI::Unitset uSet);

};
