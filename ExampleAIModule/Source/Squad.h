#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include <iostream>

class Squad : public BWAPI::Unitset {
public:
	Squad();
	~Squad();

	bool isIdle();
	bool isLastAttackPos(BWAPI::Position);

private:


	


};


