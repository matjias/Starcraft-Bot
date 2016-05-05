#pragma once
#include <BWAPI.h>

class ScoutManager {
public:
	ScoutManager();
	~ScoutManager();

	void recordUnitDiscover(BWAPI::Unit u);
	void recordUnitDestroy(BWAPI::Unit u);

private:

};
