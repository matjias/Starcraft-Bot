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
	void foundEnemyBase(BWAPI::TilePosition loc);
	


	// DEBUG METHODS BELOW

	int getX();
	int getY();

	BWAPI::TilePosition::list getSpawns();

	bool returnFoundEnemyBase();
	BWAPI::Position returnEnemyBaseLocs();
};