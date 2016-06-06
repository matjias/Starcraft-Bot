#ifdef UNITHANDLER_EXPORTS
#	define DLL_SPECIFIER _declspec(dllexport)
#else
#	define DLL_SPECIFIER _declspec(dllimport)
#endif

#pragma once
#include <BWAPI.h>


class DLL_SPECIFIER ProbeUnits {
public:
	ProbeUnits();
	~ProbeUnits();

	void addUnit(BWAPI::Unit u);

	BWAPI::Unitset* getMiningUnits();
	BWAPI::Unit extractUnit();

private:
	int miningCount;

	// Currently functioning as default unitset for probes.
	BWAPI::Unitset miningProbes;

	void mineMinerals(BWAPI::Unitset uSet);
	void mineMinerals(BWAPI::Unit u);
	void mineGas(BWAPI::Unitset uSet);

};