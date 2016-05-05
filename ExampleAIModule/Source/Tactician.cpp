#pragma once
#include "Tactician.h"

Tactician::Tactician() {
	currentStage = Start;
}

Tactician::~Tactician() { }

void Tactician::updateTactician() {
	switch (currentStage) {
	case Start:
		strategy->currentStrategy;

		break;

	case Early:


		break;

	case Mid:


		break;
	}



}
