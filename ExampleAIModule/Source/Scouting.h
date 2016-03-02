#pragma once
#include <BWAPI.h>
#include <vector>

class Scouting
{
public:
	Scouting(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc);
	~Scouting();
	bool isScouting();
	bool assignScout(BWAPI::Unit scout);
	void updateScout();
	bool endScouting();

};