#pragma once
#include <BWAPI.h>
#include "StrategyDecider.h"

class Tactician {
public:
	Tactician();
	~Tactician();

	enum GameStage {
		Start, Early, EarlyMid,
		Mid, MidLate, Late
	};

	void updateTactician();



private:
	GameStage currentStage;


	StrategyDecider *strategy;



};
