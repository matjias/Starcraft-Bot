#pragma once
#include "BWAPI.h"

class ScoutUnits {
public:
	ScoutUnits();
	~ScoutUnits();
	
	bool addUnit(BWAPI::Unit unit);
	int getAmountOfScouts();
	// std::vector<BWAPI::Unit> getScouts();
	
	struct ScoutAndGoalStruct {
		BWAPI::Unit scout;
		BWAPI::Position goal = BWAPI::Position(0, 0);
	};

	bool assignGoal(BWAPI::Position goal);

	void updateScouts();


private:
	std::vector<ScoutAndGoalStruct*> scouts;


};

