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
	bool hasAssignedScout();
	bool assignScoutandLoc(BWAPI::Unit scout, BWAPI::Position loc);
	void updateScout();
	bool endScouting();
	int getDistance();
	void foundEnemyBase(BWAPI::TilePosition loc);
	


	// DEBUG METHODS BELOW

	int getX();
	int getY();

	bool isScout(BWAPI::Unit u);

	BWAPI::Position getScout();


	BWAPI::TilePosition::list getSpawns();
	BWAPI::TilePosition::list getDynamicSpawns();
	std::vector<bool> getDynamicSpawnBools();

	bool returnFoundEnemyBase();
	BWAPI::Position returnEnemyBaseLocs();

private:
	void oneScoutAll(BWAPI::Unit u);
	void updateToScoutList();
};