#ifdef SCOUTMANAGER_EXPORTS
#	define DLL_SPECIFIER _declspec(dllexport)
#else
#	define DLL_SPECIFIER _declspec(dllimport)
#endif

#pragma once
#include "BWAPI.h"
#include "Constants.h"

class DLL_SPECIFIER ScoutUnits {
public:
	ScoutUnits();
	~ScoutUnits();
	
	bool addUnit(BWAPI::Unit unit);
	int getAmountOfScouts();
	bool hasScouts();
	
	std::vector<ScoutAndGoalStruct*> getScouts();

	bool assignGoal(BWAPI::Position goal);

	void updateScouts();


private:
	std::vector<ScoutAndGoalStruct*> scouts;


};

