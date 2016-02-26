#pragma once
#include <BWAPI.h>
#include <vector>

class Scouting
{
public:
	void init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc);
	bool isScouting();
	bool assignScout(BWAPI::Unit scout);


};