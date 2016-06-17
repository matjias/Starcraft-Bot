#pragma once
#include "Squad.h"

Squad::Squad(){}

Squad::~Squad(){}

using namespace BWAPI;

bool Squad::isIdle(){
	for (auto &unit : this->getLoadedUnits()){
		if (unit->isIdle()){
			return true;
		}
	}
}

bool Squad::isLastAttackPos(Position pos){
	for (auto &unit : this->getLoadedUnits()){
		if (unit->getLastCommand().getTargetPosition() == pos){
			return true;
		}
	}
	return false;
}

