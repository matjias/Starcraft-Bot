#include "stdafx.h"
#include "CppUnitTest.h"

#include "fakeit.hpp"

#include "..\ExampleAIModule\Source\ScoutManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace BWAPI;
using namespace fakeit;

// Declaring ToString function for Assert to use
namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> static std::wstring ToString<TilePosition>(const TilePosition & p) {
				return L"" + std::to_wstring(p.x) + L", " + std::to_wstring(p.y);
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
		// The ScoutManager_Test_Init tests concerns the _init function in ScoutManager
		// This is the function that takes all of the spawns and
		// saves and sorts the potential enemy spawn locations

		void scoutMan_Test1_Setup() {

		}

		TEST_METHOD(ScoutManager_Test_Init1) {
			// This first example is a 1v1 map, so _init should only really
			// have one way of doing this, disregarding any sorting here

			ScoutManager scoutMan;

			// Creates the starting positions
			TilePosition ownSpawn = TilePosition(1, 1);
			TilePosition enemySpawn = TilePosition(21, 21);

			TilePosition::list allSpawns;
			allSpawns.push_back(ownSpawn);
			allSpawns.push_back(enemySpawn);
			
			// Ensure spawns is empty because _init has not been called
			Assert::AreEqual((int) scoutMan.getSpawns().size(), 0);

			scoutMan._init(allSpawns, ownSpawn);
			TilePosition::list recordedSpawns = scoutMan.getSpawns();

			// Tests to make sure only the enemy's spawn is saved
			Assert::AreEqual((int) recordedSpawns.size(), 1);
			Assert::AreEqual(recordedSpawns.at(0), enemySpawn);
		}

		TEST_METHOD(ScoutManager_Test_Init2) {
			// This test will now cover a broader spawn range and
			// ensure that all potential enemy spawns are sorted
			// correctly according to their length from our own spawn

			ScoutManager scoutMan;

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

			scoutMan._init(allSpawns, ownSpawn);
			TilePosition::list recordedSpawns = scoutMan.getSpawns();

			// Now for the actual sorting tests
			Assert::AreEqual((int)recordedSpawns.size(), 3);
			
			//From actual data from starcraft, we know for certain
			//that the following sort is correct because we can do
			//a simple Euclidean check, but Starcraft might not
			//always return a straight forward distance like that
			//	  enemySpawn3
			//	  enemySpawn1
			//	  enemySpawn2
			Assert::AreEqual(recordedSpawns.at(0), enemySpawn3);
			Assert::AreEqual(recordedSpawns.at(1), enemySpawn1);
			Assert::AreEqual(recordedSpawns.at(2), enemySpawn2);
		}

		TEST_METHOD(Broodwar_Mock_Test1) {
			Mock<Game> Broodwar_Mock;
			Mock<PlayerInterface> Self_Mock;
			//Mock<Game> Broodwar;

			TilePosition s = TilePosition(1, 1);
			TilePosition ss = TilePosition(2, 2);
			TilePosition::list spawns;
			spawns.push_back(s);
			spawns.push_back(ss);

			When(Method(Self_Mock, getStartLocation)).AlwaysReturn(s);
			PlayerInterface &p = Self_Mock.get();
			
			Assert::AreEqual(p.getStartLocation(), s);

			When(Method(Broodwar_Mock, self)).AlwaysReturn(&p);
			When(Method(Broodwar_Mock, getStartLocations)).AlwaysReturn(spawns);
			
			//Game &Broodwar = Broodwar_Mock.get();
			Game &broodwar = Broodwar_Mock.get();
			
			Assert::AreEqual(broodwar.self()->getStartLocation(), s);
			Assert::AreEqual(broodwar.getStartLocations(), spawns);

			ScoutManager scoutMan = ScoutManager(&broodwar);
			scoutMan._initT();

			TilePosition::list recordedSpawns = scoutMan.getSpawns();
			Assert::AreEqual(recordedSpawns.at(0), ss);


		}

	};
}