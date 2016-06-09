#include "stdafx.h"
#include "CppUnitTest.h"

#include "fakeit.hpp"

#include "..\ExampleAIModule\Source\ScoutManager.h"
#include "..\ExampleAIModule\Source\Tactician.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace BWAPI;
using namespace fakeit;

// Declaring ToString function for Assert to use
namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> static std::wstring ToString<Position>(const Position & p) {
				return L"" + std::to_wstring(p.x) + L"," + std::to_wstring(p.y);
			}

			template<> static std::wstring ToString<TilePosition>(const TilePosition & p) {
				return L"" + std::to_wstring(p.x) + L"," + std::to_wstring(p.y);
			}

			template<> static std::wstring ToString<TilePosition::list>(const TilePosition::list & p) {
				std::wstring returnStr = L"";
				
				for (auto & s : p) {
					returnStr += std::to_wstring(s.x) + L"," + std::to_wstring(s.y) + L";";
				}
				
				return returnStr;
			}


		}
	}
}

namespace UnitTest {
	TEST_CLASS(UnitTest1) {
	public:
		// The first test here is used for testing the use of
		// mocking the Broodwar game pointer, along with a player
		// and ensuring that they return something we can test
		TEST_METHOD(Broodwar_Mock_Test1) {
			Mock<Game> Broodwar_Mock;
			Mock<PlayerInterface> Self_Mock;

			TilePosition ownSpawn = TilePosition(1, 1);
			TilePosition enemySpawn = TilePosition(2, 2);
			TilePosition::list allSpawns;
			allSpawns.push_back(ownSpawn);
			allSpawns.push_back(enemySpawn);

			// Here we mock the methods to return what we want
			When(Method(Self_Mock, getStartLocation)).AlwaysReturn(ownSpawn);
			When(Method(Broodwar_Mock, getStartLocations)).AlwaysReturn(allSpawns);
			PlayerInterface &self = Self_Mock.get();
			When(Method(Broodwar_Mock, self)).AlwaysReturn(&self);

			// Finally test if they are returning
			// what we mocked them to
			Assert::AreEqual(ownSpawn, self.getStartLocation());

			Game* broodwar = &Broodwar_Mock.get();

			Assert::AreEqual(ownSpawn, broodwar->self()->getStartLocation());
			Assert::AreEqual(allSpawns, broodwar->getStartLocations());
		}

		// The ScoutManager_Test_Init tests concerns the _init 
		// function in ScoutManager
		// This is the function that takes all of the spawns and
		// saves and sorts the potential enemy spawn locations

		// The following two tests concern the behaviour
		// of ScoutManager's _init() function
		TEST_METHOD(ScoutManager_Init1) {
			// This first example is a 1v1 map, so the _init
			// behaviour should be that there is only one spawn
			// in it's list of potential spawns, and that it should
			// know where the enemy is
			Mock<Game> Broodwar_Mock;
			Mock<PlayerInterface> Self_Mock;

			// Creates the starting positions
			TilePosition ownSpawn = TilePosition(1, 1);
			TilePosition enemySpawn = TilePosition(21, 21);

			TilePosition::list allSpawns;
			allSpawns.push_back(ownSpawn);
			allSpawns.push_back(enemySpawn);
			
			When(Method(Self_Mock, getStartLocation)).AlwaysReturn(ownSpawn);
			When(Method(Broodwar_Mock, getStartLocations)).AlwaysReturn(allSpawns);
			PlayerInterface &self = Self_Mock.get();
			When(Method(Broodwar_Mock, self)).AlwaysReturn(&self);
			Game* broodwar = &Broodwar_Mock.get();

			ScoutManager scoutMan = ScoutManager(broodwar);

			// Ensure spawns is empty because _init has not been called
			Assert::AreEqual(0, (int) scoutMan.getSpawns().size());

			scoutMan._init();
			TilePosition::list recordedSpawns = scoutMan.getSpawns();

			// Tests to make sure only the enemy's spawn is saved
			Assert::AreEqual(1, (int) recordedSpawns.size());
			Assert::AreEqual(enemySpawn, recordedSpawns.at(0));
			Assert::AreEqual(enemySpawn, scoutMan.getEnemySpawn());
		}

		TEST_METHOD(ScoutManager_Init2) {
			// This test will now cover a broader spawn range and
			// ensure that all potential enemy spawns are sorted
			// correctly according to their length from our own spawn
			Mock<Game> Broodwar_Mock;
			Mock<PlayerInterface> Self_Mock;

			// Creates the starting positions
			// These spawn locations are taken from (4)Jade.scx
			// which is one of the maps from the sscai map pool
			TilePosition ownSpawn = TilePosition(8, 117);
			TilePosition enemySpawn1 = TilePosition(7, 7);
			TilePosition enemySpawn2 = TilePosition(117, 7);
			TilePosition enemySpawn3 = TilePosition(117, 117);
			

			TilePosition::list allSpawns;
			allSpawns.push_back(ownSpawn);
			allSpawns.push_back(enemySpawn1);
			allSpawns.push_back(enemySpawn2);
			allSpawns.push_back(enemySpawn3);

			When(Method(Self_Mock, getStartLocation)).AlwaysReturn(ownSpawn);
			When(Method(Broodwar_Mock, getStartLocations)).AlwaysReturn(allSpawns);
			PlayerInterface &self = Self_Mock.get();
			When(Method(Broodwar_Mock, self)).AlwaysReturn(&self);
			Game &broodwar = Broodwar_Mock.get();

			ScoutManager scoutMan = ScoutManager(&broodwar);
			scoutMan._init();
			TilePosition::list recordedSpawns = scoutMan.getSpawns();

			// Now for the actual sorting tests
			Assert::AreEqual(3, (int) recordedSpawns.size());
			
			//The following sort is the correct one based on data
			//directly from Starcraft and if we did a euclidean
			//check, we would find this to be true as well.
			//A little note however is that it uses a
			//getApproxDistance function which we don't know
			//the exact behaviour of
			//	  enemySpawn3
			//	  enemySpawn1
			//	  enemySpawn2
			Assert::AreEqual(enemySpawn3, recordedSpawns.at(0));
			Assert::AreEqual(enemySpawn1, recordedSpawns.at(1));
			Assert::AreEqual(enemySpawn2, recordedSpawns.at(2));
		}

		TEST_METHOD(ScoutManager_UpdateList1) {
			Mock<Game> Broodwar_Mock;
			Mock<PlayerInterface> Self_Mock;
			Mock<UnitInterface> Unit_Mock;

			TilePosition ownSpawn = TilePosition(8, 117);
			TilePosition enemySpawn1 = TilePosition(7, 7);
			TilePosition enemySpawn2 = TilePosition(117, 7);
			TilePosition enemySpawn3 = TilePosition(117, 117);

			TilePosition::list allSpawns;
			allSpawns.push_back(ownSpawn);
			allSpawns.push_back(enemySpawn1);
			allSpawns.push_back(enemySpawn2);
			allSpawns.push_back(enemySpawn3);
			

			When(Method(Self_Mock, getStartLocation)).AlwaysReturn(ownSpawn);
			When(Method(Broodwar_Mock, getStartLocations)).AlwaysReturn(allSpawns);
			PlayerInterface &self = Self_Mock.get();
			When(Method(Broodwar_Mock, self)).AlwaysReturn(&self);
			Fake(Method(Broodwar_Mock, vSendTextEx));

			When(ConstOverloadedMethod(
				Broodwar_Mock,
				isVisible,
				bool(int, int)
				).Using(enemySpawn1.x, enemySpawn1.y)
			).AlwaysReturn(false);

			When(ConstOverloadedMethod(
				Broodwar_Mock,
				isVisible,
				bool(int, int)
				).Using(enemySpawn2.x, enemySpawn2.y)
			).AlwaysReturn(false);

			When(ConstOverloadedMethod(
				Broodwar_Mock,
				isVisible,
				bool(int, int)
				).Using(enemySpawn3.x, enemySpawn3.y)
			).Return(false, true);


			
			Game &broodwar = Broodwar_Mock.get();

			ScoutManager scoutMan = ScoutManager(&broodwar);
			scoutMan._init();

			ScoutUnits scoutUnits;

			When(Method(Unit_Mock, exists)).AlwaysReturn(true);
			When(Method(Unit_Mock, isCompleted)).AlwaysReturn(true);
			When(Method(Unit_Mock, getType)).AlwaysReturn(UnitTypes::Protoss_Probe);
			
			TilePosition closeEnemySpawn3 = TilePosition(enemySpawn3.x - 20, enemySpawn3.y);
			When(Method(Unit_Mock, getPosition))
				.Return(Position(ownSpawn), Position(ownSpawn),
						Position(closeEnemySpawn3), Position(closeEnemySpawn3),
						Position(enemySpawn3), Position(enemySpawn3));

			
			
			Unit u = &Unit_Mock.get();

			scoutUnits.addUnit(u);
			Assert::IsTrue(scoutMan.beginScouting(&scoutUnits));

			// Fetch has scout on the spawns
			std::vector<bool> hasScouts = scoutMan.getSpawnHasScouts();
			Assert::IsTrue(hasScouts.at(0));
			Assert::IsFalse(hasScouts.at(1));

			TilePosition::list recordedSpawns = scoutMan.getSpawns();
			
			
			// Scout is in own spawn now
			scoutMan.updateScoutManager(); 
			// Scout is now in closeEnemySpawn3

			// Scout is now in closeEnemySpawn3
			scoutMan.updateScoutManager();
			// Scout is now in enemySpawn3
			
			// Now we should have scouted the enemySpawn3 and the enemy was not there
			// so now it should update the list to move enemySpawn3 all the way back
			recordedSpawns = scoutMan.getSpawns();
			Assert::AreEqual(enemySpawn1, recordedSpawns.at(0));
			Assert::AreEqual(enemySpawn2, recordedSpawns.at(1));
			Assert::AreEqual(enemySpawn3, recordedSpawns.at(2));

			// However, when it updates again, the scout manager should
			// determine that enemySpawn2 is actually closer to the current
			// scout, and update the goal
			scoutMan.updateScoutManager();

			recordedSpawns = scoutMan.getSpawns();
			Assert::AreEqual(enemySpawn2, recordedSpawns.at(0));
			Assert::AreEqual(enemySpawn1, recordedSpawns.at(1));
			Assert::AreEqual(enemySpawn3, recordedSpawns.at(2));
		}

		TEST_METHOD(Mining_Probes_Test) {
			UnitHandler handler;
			Mock<UnitInterface> UnitInt_Mock;
			
			When(Method(UnitInt_Mock, getType)).AlwaysReturn(UnitTypes::Protoss_Probe);
			When(Method(UnitInt_Mock, getID)).AlwaysReturn(12); 

			Unit unit = &UnitInt_Mock.get();

			handler.addUnit(unit);
			Unitset* mineProbes = handler.getProbeUnits()->getMiningUnits();
			Unitset::iterator it = mineProbes->begin();
			Unit u = *it;

			Assert::AreEqual(u->getID(), 12);
		}

		TEST_METHOD(BuildingUnits_Test_Init1) {
			//Tactician tactician;

		}

		TEST_METHOD(Probe_Deletion){
			
			UnitHandler 




		}
	};
}