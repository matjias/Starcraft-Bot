#pragma once
#include <BWAPI.h>



class Army
{
public:
	Army();
	~Army();

	void _init();

	bool buildZealot(BWAPI::Unit u);
	void addZealot(BWAPI::Unit u);
	void run(Scouting scoutClass);

private:


};


