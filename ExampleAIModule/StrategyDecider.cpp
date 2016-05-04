#pragma once
#include "StrategyDecider.h"

StrategyDecider::StrategyDecider() { }

StrategyDecider::~StrategyDecider() { }

void StrategyDecider::decideStrategy() {
	if (needsToUpdateStrategy) {
		currentStrategy = Default;
		needsToUpdateStrategy = false;
	}
}