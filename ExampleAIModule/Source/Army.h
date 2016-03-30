#pragma once
#include <BWAPI.h>
#include "Scouting.h"


class Army
{
public:
	Army();
	~Army();

	void _init();

	void update(Scouting scoutClass);
	bool buildZealot(BWAPI::Unit u);
	void addZealot(BWAPI::Unit u);
	void attack(Scouting scoutClass);
	void zealotRush();

private:
	void attack();

};


