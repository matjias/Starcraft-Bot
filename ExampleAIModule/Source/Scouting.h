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
	bool assignScoutandLoc(BWAPI::Unit scout, BWAPI::Position loc);
	void updateScout();
	bool endScouting();
	int getDistance();
	void foundEnemyBase(BWAPI::TilePosition loc);
	


	// DEBUG METHODS BELOW

	int getX();
	int getY();
<<<<<<< HEAD
	bool isScout(BWAPI::Unit u);
=======
	BWAPI::Position getScout();
>>>>>>> 3ca99eb77015b46df852b768dcd8173b8b830949

	BWAPI::TilePosition::list getSpawns();
	BWAPI::TilePosition::list getDynamicSpawns();

	bool returnFoundEnemyBase();
	BWAPI::Position returnEnemyBaseLocs();

private:
	void oneScoutAll(BWAPI::Unit u);
	void updateToScoutList();
};