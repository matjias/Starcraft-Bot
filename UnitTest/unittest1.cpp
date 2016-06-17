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

		// This test covers the use of the Broodwar keyword in the classes
		// that we wish to override in the unit tests.
		TEST_METHOD(Broodwar_Mock_Test2) {
			Mock<Game> Broodwar_Mock;
			Mock<PlayerInterface> Player_Mock;

			When(Method(Player_Mock, minerals)).AlwaysReturn(50);
			Player p = &Player_Mock.get();

			When(Method(Broodwar_Mock, self)).AlwaysReturn(p);
			BroodwarPtr = &Broodwar_Mock.get();

			ScoutManager scoutMan;
			scoutMan.setBroodwarMock(BroodwarPtr);

			Player pp = scoutMan.testBroodwarReturnPlayer();
			int p1 = p->minerals();
			int p2 = pp->minerals();

			Assert::AreEqual(p1, p2);
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

			ScoutManager scoutMan;

			// Ensure spawns is empty because _init has not been called
			Assert::AreEqual(0, (int) scoutMan.getSpawns().size());

			scoutMan.setBroodwarMock(broodwar);
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
			Game* broodwar = &Broodwar_Mock.get();

			ScoutManager scoutMan;
			scoutMan.setBroodwarMock(broodwar);
			scoutMan._init();
			TilePosition::list recordedSpawns = scoutMan.getSpawns();

			// Now for the actual sorting tests
			Assert::AreEqual(3, (int) recordedSpawns.size());
			
			//The following sort is the correct one based on data
			//directly from Starcraft and if we did a euclidean
			//check, we would find this to be true as well.
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
			
			Game* broodwar = &Broodwar_Mock.get();

			ScoutManager scoutMan;
			scoutMan.setBroodwarMock(broodwar);
			scoutMan._init();

			ScoutUnits scoutUnits;

			When(Method(Unit_Mock, exists)).AlwaysReturn(true);
			When(Method(Unit_Mock, isCompleted)).AlwaysReturn(true);
			When(Method(Unit_Mock, getType)).AlwaysReturn(UnitTypes::Protoss_Probe);
			
			TilePosition closeEnemySpawn3 = TilePosition(enemySpawn3.x - 20, 
														 enemySpawn3.y);
			When(Method(Unit_Mock, getPosition))
				.Return(Position(ownSpawn),
						Position(closeEnemySpawn3),
						Position(enemySpawn3));
			
			Unit u = &Unit_Mock.get();

			scoutUnits.addUnit(u);
			Assert::IsTrue(scoutMan.beginScouting(&scoutUnits));

			// The list should be the same as earlier tests
			TilePosition::list recordedSpawns = scoutMan.getSpawns();
			Assert::AreEqual(enemySpawn3, recordedSpawns.at(0));
			Assert::AreEqual(enemySpawn1, recordedSpawns.at(1));
			Assert::AreEqual(enemySpawn2, recordedSpawns.at(2));

			scoutMan.updateScoutManager(); 
			recordedSpawns = scoutMan.getSpawns();

			// Scout is now in closeEnemySpawn3, nothing should change
			Assert::AreEqual(enemySpawn3, recordedSpawns.at(0));
			Assert::AreEqual(enemySpawn1, recordedSpawns.at(1));
			Assert::AreEqual(enemySpawn2, recordedSpawns.at(2));

			// Scout is now in enemySpawn3
			scoutMan.updateScoutManager();
			recordedSpawns = scoutMan.getSpawns();

			// Scout is now in enemySpawn3, so it should be scouted now
			// The enemy was not in enemySpawn3, so we push it to the back
			Assert::AreEqual(enemySpawn1, recordedSpawns.at(0));
			Assert::AreEqual(enemySpawn2, recordedSpawns.at(1));
			Assert::AreEqual(enemySpawn3, recordedSpawns.at(2));

			scoutMan.updateScoutManager();
			recordedSpawns = scoutMan.getSpawns();

			// Scout show now be closer to enemySpawn2 than enemySpawn1
			// so the scout should be reassigned to there, and the list
			// updated because any spawns that have a scout are pushed to
			// the front of the list
			Assert::AreEqual(enemySpawn2, recordedSpawns.at(0));
			Assert::AreEqual(enemySpawn1, recordedSpawns.at(1));
			Assert::AreEqual(enemySpawn3, recordedSpawns.at(2));

			/*s1 = std::to_wstring(recordedSpawns.at(0).x) + L", " +
			std::to_wstring(recordedSpawns.at(0).y);
			s2 = std::to_wstring(recordedSpawns.at(1).x) + L", " +
			std::to_wstring(recordedSpawns.at(1).y);
			s3 = std::to_wstring(recordedSpawns.at(2).x) + L", " +
			std::to_wstring(recordedSpawns.at(2).y);
			Logger::WriteMessage(s1.c_str());
			Logger::WriteMessage(s2.c_str());
			Logger::WriteMessage(s3.c_str());*/
		}

		TEST_METHOD(Mining_Probes_Test) {
			UnitHandler handler;
			Mock<Game> Broodwar_Mock;

			Mock<UnitInterface> Probe_Mock;
			Mock<UnitInterface> Nexus_Mock;
			Mock<UnitInterface> MineralField_Mock;

			int probeID = 15;
			int nexusID = 7;
			int mineralFieldID = 3;

			Position probePos = Position(224, 288);
			Position nexusPos = Position(224, 224);
			Position fieldPos = Position(128, 224);
			
			When(Method(Probe_Mock, getType)).AlwaysReturn(UnitTypes::Protoss_Probe);
			When(Method(Probe_Mock, getID)).AlwaysReturn(probeID);
			Fake(Method(Probe_Mock, issueCommand));
			When(Method(Probe_Mock, exists)).AlwaysReturn(true);
			When(Method(Probe_Mock, getPosition)).AlwaysReturn(probePos);

			When(Method(Nexus_Mock, getID)).AlwaysReturn(nexusID);
			When(Method(Nexus_Mock, getPosition)).AlwaysReturn(nexusPos);
			When(Method(MineralField_Mock, getID)).AlwaysReturn(mineralFieldID);
			When(Method(MineralField_Mock, getPosition)).AlwaysReturn(fieldPos);

			Unit nexus = &Nexus_Mock.get();
			Unit mineralField = &MineralField_Mock.get();

			When(ConstOverloadedMethod(
					Broodwar_Mock, 
					getClosestUnitInRectangle,
					Unit(Position, const UnitFilter&, int, int, int, int)
				)/*.Using(probePos, 
						Filter::GetType == UnitTypes::Protoss_Nexus, 
						)*/
			).Return(nexus, mineralField);

			Game* broodwar = &Broodwar_Mock.get();

			handler.setBroodwarMock(broodwar);
			handler._init();

			Unit unit = &Probe_Mock.get();
			handler.addUnit(unit);

			Unitset* mineProbes = handler.getProbeUnits()->getMiningUnits();
			Unitset::iterator it = mineProbes->begin();
			Unit u = *it;

			Assert::AreEqual(probeID, u->getID());
		
		}

		TEST_METHOD(Move_Probes_Test) {
			Unitset set1;
			Unitset set2;
			
			int setSize = 3;

			Mock<UnitInterface> Temp_Mock_Unit1;
			When(Method(Temp_Mock_Unit1, getID)).AlwaysReturn(1);
			Unit tempU1 = &Temp_Mock_Unit1.get();
			set1.insert(tempU1);

			Mock<UnitInterface> Temp_Mock_Unit2;
			When(Method(Temp_Mock_Unit2, getID)).AlwaysReturn(2);
			Unit tempU2 = &Temp_Mock_Unit2.get();
			set1.insert(tempU2);

			Mock<UnitInterface> Temp_Mock_Unit3;
			When(Method(Temp_Mock_Unit3, getID)).AlwaysReturn(3);
			Unit tempU3 = &Temp_Mock_Unit3.get();
			set1.insert(tempU3);

			ProbeUnits probeUnits;

			probeUnits.moveUnits(&set1, &set2, setSize);

			int j = 1;
			for (auto &unit : set2){
				std::wstring s1 = std::to_wstring(j);
				Logger::WriteMessage(s1.c_str());
				Assert::AreEqual(unit->getID(), j);
				j++;
			}


		}

		//TEST_METHOD(BuildingUnits_Test_Init1) {
		//	//Tactician tactician;

		//}

		//TEST_METHOD(Probe_Deletion){
		//	
		//	//UnitHandler 




		//}



		
	};
}