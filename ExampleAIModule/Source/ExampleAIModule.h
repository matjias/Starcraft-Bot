#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>

#include "StrategyDecider.h"
#include "Tactician.h"
#include "ScoutManager.h"




// Remember not to use "Broodwar" in any global class constructor!

class ExampleAIModule : public BWAPI::AIModule {
public:
	// Virtual functions for callbacks, leave these as they are.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);

	// Everything below this line is safe to modify.

	// BWTA2 function
	void drawTerrainData();

	// Our functions
	void drawData();

	// Our global variables
	// BWTA variables, need to be here
	bool analyzed;
	bool analysis_just_finished;
	
	// The classes that the AI Module holds
	StrategyDecider strategyDecider;
	Tactician tactician;
	ScoutManager scoutManager;
	
};


// BWTA Thread Function
DWORD WINAPI AnalyzeThread(ExampleAIModule *para);
