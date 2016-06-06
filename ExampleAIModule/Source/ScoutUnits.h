#pragma once
#include "BWAPI.h"
#include "Constants.h"

class ScoutUnits {
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

