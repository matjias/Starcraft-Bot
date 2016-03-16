#pragma once
#include <BWAPI.h>
#include "Scouting.h"


class Army
{
public:
	Army();
	~Army();

	void _init();

	bool buildZealot(BWAPI::Unit u);
	void addZealot(BWAPI::Unit u);
	void run(Scouting scoutClass);
	void attack();

private:


};


