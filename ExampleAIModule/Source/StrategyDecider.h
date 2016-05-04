#pragma once
#include "ScoutManager.h"


class StrategyDecider {
public:
	StrategyDecider();
	~StrategyDecider();


	enum StrategyName {
		// Default has no direct goal in mind, but just works towards
		// getting economy, teching up, building an army and harassing
		// where possible
		Default,
		AllIn, Defend,
		Expand, Turtle
	};

	void decideStrategy();

	bool needsToUpdateStrategy = true;
	StrategyName currentStrategy;

private:
	ScoutManager* scout;
};
