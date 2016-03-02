#pragma once
#include <BWAPI.h>
#include <vector>

class Scouting {
public:
	Scouting();
	~Scouting();
	void _init(BWAPI::TilePosition::list locs, BWAPI::TilePosition loc);
	bool isScouting();
	bool assignScout(BWAPI::Unit scout);
	void updateScout();
	bool endScouting();
	int getDistance();


	int getX();
	int getY();


};