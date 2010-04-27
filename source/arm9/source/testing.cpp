/**
 * @file testing.cpp
 *
 * @brief Testing framework main function
 *
 * This file contains the main() function that takes over when testing is enabled in the zoidberg engine.
 *
 * @author Joe Balough
 */

/**
 *  Copyright (c) 2010 zoidberg engine
 *
 *  This file is part of the zoidberg engine.
 *
 *  The zoidberg engine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The zoidberg engine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the zoidberg engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef ZBE_TESTING

#include <nds.h>
#include <maxmod9.h>
#include <fat.h>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include "game.h"
#include "util.h"

/**
 *    GLOBAL VARIABLES
 */
// Total number of tests run
uint16 testsRun = 0;
// Total number of failed tests run
uint16 testsFailed = 0;


using namespace std;

/**
 *    FUNCTIONAL TESTS
 */

/**
 * Abstract functionalTest class. To add a new functional test, create a new subclass of this abstract class.
 * @author Joe Balough
 */
class functionalTest
{
public:
	functionalTest() {}
	~functionalTest() {}

	/**
	 * A method to run the functional test
	 * should print whether the test was successful and pause before returning
	 * @return bool
	 *   Whether or not the test was successful
	 */
	virtual bool run()
	{
		iprintf("Whoops! You defined your\nfunctionalTest wrong.\n");
		pauseIfTesting();
		return false;
	}

	/**
	 * getName() function; returns the name of this test
	 * @author Joe Balough
	 */
	inline string getName()
	{
		return name;
	}

protected:
	/**
	 * A string representing the name of this functional test. Should be like "Part Test".
	 */
	string name;
};


/**
 * collisionMatrixTest
 *
 * A functional test to test the workings of the collisionMatrix class
 *
 * @author Joe Balough
 */
class collisionMatrixTest : public functionalTest
{
public:

	/**
	 * Constructor; just sets the test name
	 * @author Joe Balough
	 */
	collisionMatrixTest()
	{
		name = "collisionMatrix Test";
	}

	/**
	 * Test run function
	 *
	 *
	 *
	 * @ author Joe Balough
	 */
	virtual bool run()
	{
		//       --------------------------------
		iprintf("collisionMatrix functional Test\n\n");

		iprintf("Making 10 x 10 px @ 5px / group\n");
		iprintf("       collisionMatrix.\n");
		collisionMatrix mat(10, 10, 5);

		iprintf("Making some objects\n");
		vector2D<float> objPos[5];
		objPos[0] = vector2D<float>(2.5, 2.5);  //   _________
		objPos[1] = vector2D<float>(7.5, 2.5);  //  |0&4 |1   |
		objPos[2] = vector2D<float>(2.5, 7.5);  //  |____|____|
		objPos[3] = vector2D<float>(7.5, 7.5);  //  |2   |3   |
		objPos[4] = vector2D<float>(2.5, 2.5);  //  |____|____|

		object *objects[5];
		for (int i = 0; i < 5; i++)
		{
			objects[i] = new object(objPos[i]);
			if (!objects[i])
			{
				//       --------------------------------
				iprintf("\nFailed to create new object.\n");
				iprintf("new returned NULL\n");
				iprintf("Out of memory?\n");
				iprintf("Test failed.\n");
				pauseIfTesting();
				return false;
			}
		}


		//       --------------------------------
		iprintf("\n   Testing cM construction\n\n");


		iprintf("testing cM::addObject()\n");
		objGroup *objGroups[5];
		for (int i = 0; i < 5; i ++)
		{
			objGroups[i] = mat.addObject(objects[i]);

			if (!objGroups[i])
			{
				iprintf("\nFailed to add object to matrix\n");
				iprintf("cM::addObject() returned NULL\n");
				iprintf("Test failed.\n");
				pauseIfTesting();
				return false;
			}
		}

		// Make sure that object[0] and object[4] are in the same group
		if (objGroups[0] != objGroups[4])
		{
			//       --------------------------------
			iprintf("obj0 and obj5 not in same group\n");
			iprintf("Test failed.\n");
			pauseIfTesting();
			return false;
		}


		// See if they're all appropriately sized
		if (objGroups[0]->objects.size() != 2)
		{
			iprintf("objGroup 0 has %d objects\n", objGroups[0]->objects.size());
			iprintf("should be 2\n");
			iprintf("Test failed.\n");
			pauseIfTesting();
			return false;
		}
		// don't do group 0 or group 4
		for (int i = 1; i < 4; i++)
		{
			if (objGroups[i]->objects.size() != 1)
			{
				iprintf("objGroup %d has %d objects\n", i, objGroups[i]->objects.size());
				iprintf("should be 1\n");
				iprintf("Test failed.\n");
				pauseIfTesting();
				return false;
			}
		}


		iprintf("testing cM::getObjGroup()\n");
		for (int i = 0; i < 5; i++)
		{
			// Get the object group for each of those objects, see if they're the same as addObject returned
			if (mat.getObjGroup(objPos[i]) != objGroups[i])
			{
				iprintf("\nGot a different objGroup for\n");
				iprintf("object %d.\n", i);
				iprintf("Test failed.\n");
				pauseIfTesting();
				return false;
			}
		}




		iprintf("\n        Moving object,\n  tesing cM object reinsertion\n\n");



		iprintf("moving first object\n");
		objPos[0] = vector2D<float>(7.5, 7.5);
		objects[0]->position = objPos[0];

		// Remove that object from its old collision matrix and re-insert into the collisionMatrix
		iprintf("testing oJ::remove()\n");
		objGroups[0]->remove(objects[0]);
		iprintf("testing cM::addObject()\n");
		objGroups[0] = mat.addObject(objects[0]);



		// Check its new group

		// Make sure that object[0] and object[3] are in the same group
		if (objGroups[0] != objGroups[3])
		{
			//       --------------------------------
			iprintf("obj0 and obj3 not in same group\n");
			iprintf("Test failed.\n");
			pauseIfTesting();
			return false;
		}

		// See if they're all appropriately sized
		if (objGroups[0]->objects.size() != 2)
		{
			iprintf("objGroup 0 has %d objects\n", objGroups[0]->objects.size());
			iprintf("should be 2\n");
			iprintf("Test failed.\n");
			pauseIfTesting();
			return false;
		}
		for (int i = 1; i < 5; i++)
		{
			// skip the 3rd group
			if ( i == 3 ) continue;

			if (objGroups[i]->objects.size() != 1)
			{
				iprintf("objGroup %d has %d objects\n", i, objGroups[i]->objects.size());
				iprintf("should be 1\n");
				iprintf("Test failed.\n");
				pauseIfTesting();
				return false;
			}
		}

		iprintf("\n        Cleaning up\n");
		for (int i = 0; i < 5; i++)
			delete objects[i];
		iprintf("\n       Test successful.\n");

		pauseIfTesting();
		return true;
	}

private:

};











/**
 *     FUNCTIONAL TEST FUNCTIONS
 */



/**
 * getFunctionalTests() function
 *
 * Sets up a vector of functionalTests that can be run.
 * If you add a new functional test, make sure to add it to the vector here.
 *
 * @param vector<functionalTest*> &tests
 *   A vector of a list of pointers to functionalTests to fill.
 *   Passed by reference to avoid excessive copies
 * @author Joe Balough
 */
void getFunctionalTests(vector<functionalTest*> &tests)
{
	// Add the collisonMatrix test
	collisionMatrixTest *cmt = new collisionMatrixTest;
	tests.push_back((functionalTest*) cmt);

	// TODO: ADD YOUR CUSTOM FUNCTIONAL TESTS HERE

}

/**
 * clearFunctionalTests() function
 *
 * Clears a vector full of functionalTests. Run this on the vector obtained with
 * getFunctionalTests() before returning to avoid memory leaks.
 *
 * @see getFunctionalTests
 * @param vector<functionalTest*> &test
 *   A vector of a list of pointers to functionalTests to clear.
 *   Passed by reference to avoid excessive copies
 * @param author Joe Balough
 */
void clearFunctionalTests(vector<functionalTest*> &tests)
{
	// Delete every one
	for (unsigned int i = 0; i < tests.size(); i++)
		delete tests[i];

	// Clear the vector
	tests.clear();
}


/**
 * functionalTestMenu();
 *
 * shows a menu to the user listing all of the functional tests that can be run.
 *
 * @author Joe Balough
 */
void functionalTestMenu()
{
	// Get the list of functional tests
	vector<functionalTest*> tests;
	getFunctionalTests(tests);

	// Build a list of strings for the menu
	vector<string> list;
	for (unsigned int i = 0; i < tests.size(); i++)
	{
		list.push_back(tests[i]->getName());
	}
	list.push_back("Return to Main");

	// Run the tests until the user selects the last option in the list
	unsigned int choice = menu(list, "Please select a \nfunctional test to run:\n");
	while (choice != list.size() - 1)
	{
		// Run the chosen test
		if (tests[choice]->run())
			++testsRun;
		else
		{
			++testsRun;
			++testsFailed;
		}

		// Report on overall numer of tests run
		stringstream message;
		message << testsRun - testsFailed << " out of " << testsRun << " Tests Passed\n\n";

		// Let the user pick a different test
		message << "Please select a \nfunctional test to run:\n";
		choice = menu(list, message.str());
	}

	// Delete all the functional tests made for that vector
	clearFunctionalTests(tests);
}


/**
 * runFunctionalTests function
 *
 * Runs all defined functional tests. After running all tests, it reports how many failed
 *
 * @author Joe Balough
 */
void runFunctionalTests()
{
	// Get the list of functional tests
	vector<functionalTest*> tests;
	getFunctionalTests(tests);

	uint16 oldTestsFailed = testsFailed;
	uint16 oldTestsRun = testsRun;
	testsRun = testsFailed = 0;

	// Run every test
	for (unsigned int i = 0; i < tests.size(); i++)
	{
		if (tests[i]->run())
			++testsRun;
		else
		{
			++testsRun;
			++testsFailed;
		}
	}

	// Delete all the functional tests made for that vector
	clearFunctionalTests(tests);

	// Print results and pause
	iprintf("%d out of %d functional\ntests passed.\n\n", testsRun - testsFailed, testsRun);
	pauseIfTesting();

	testsRun += oldTestsRun;
	testsFailed += oldTestsFailed;
}














/**
 *           GRAPHICAL TEST FUNCTIONS
 */




/**
 * Function runGraphicalTest
 *
 * Runs a single graphical test.
 * A graphical test is just a level of the game which is run for a certain number of screen blanks.
 * After that time period is up, it returns here where the user is asked if the test ran as it should have
 *
 * @param game *tests
 *   A pointer to the game object used for tests
 * @param uint32 testNo
 *   The test number (level number) to run
 * @author Joe Balough
 */
void runGraphicalTest(game *tests, uint32 testNo)
{
	// Run the test
	tests->runLevel(testNo);

	// Ask the user if it ran as described
	if (yesNoMenu("Did that test run correctly?\n"))
		++testsRun;
	else
	{
		++testsRun;
		++testsFailed;

		string dbgMsg = tests->getDebugMessage(testNo);
		iprintf("%s\n\n", dbgMsg.c_str());
		pauseIfTesting();
	}
}


/**
 * graphicalTestMenu function
 *
 * Displays a list of graphical tests and allows the user to run any one of them.
 *
 * @param game *g
 *   A pointer to the testing game object
 * @author Joe Balough
 */
void graphicalTestMenu(game *g)
{
	// Get list of level names (test names)
	vector<string> list;
	g->getLevelNames(list);

	// Add Quit option to that list
	list.push_back("Return to Main");

	// Run the tests until the user selects the last option in the list
	unsigned int choice = menu(list, "Please select a \ngraphical test to run:\n");
	while (choice != list.size() - 1)
	{
		// Run the chosen test
		runGraphicalTest(g, choice);

		// Report on overall numer of tests run
		stringstream message;
		message << testsRun - testsFailed << " out of " << testsRun << " Tests Passed\n\n";

		// Let the user pick a different test
		message << "Please select a \nfunctional test to run:\n";
		choice = menu(list, message.str());
	}

}


/**
 * runGraphicalTests function
 *
 * Runs all graphical tests in a row. Afterwards, it reports how many tests failed.
 *
 * @param game *g
 *   A pointer to the testing game object
 * @author Joe Balough
 */
void runGraphicalTests(game *g)
{
	uint16 oldTestsFailed = testsFailed;
	uint16 oldTestsRun = testsRun;
	testsRun = testsFailed = 0;

	// Get the number of tests there are
	uint32 numTests = g->numLevels();

	// Run all the tests
	for (uint32 i = 0; i < numTests; i++)
	{
		runGraphicalTest(g, i);
	}

	// Report the number failed and pause
	iprintf("%d out of %d graphical\ntests passed.\n\n", testsRun - testsFailed, testsRun);
	pauseIfTesting();

	testsRun += oldTestsRun;
	testsFailed += oldTestsFailed;
}




















/**
 *     MAIN FUNCTION
 */
int main()
{
	initVideo();

	// Initialize libfat
	fatInitDefault();

	// Tell user what's happening
	//       --------------------------------
	iprintf("ZOIDBERG ENGINE TESTING BUILD\n\n");
	iprintf("Now opening the zbe testing file\n\n");
	iprintf("Parsing information will be\npresented so that you can\nconfirm it was all parsed and\nloaded correctly.\n\n");
	pauseIfTesting();

	// Initialize the testing game
	game g((char *) "/testing.zbe");

	// Print some debug information for the user
	string msg = "Game data parsing successful.\n\nPlease select a test to run:\n";

	// Build a menu and run it
	vector<string> list;
	list.push_back("Run All Tests");
	list.push_back("Run a Functional Test");
	list.push_back("Run a Graphical Test");

	while (true)
	{
		int choice = menu(list, msg);

		switch (choice)
		{
			case 0:
				runFunctionalTests();
				consoleClear();
				runGraphicalTests(&g);
				consoleClear();
				iprintf("All tests complete.\n%d out of %d tests passed.\n", testsRun - testsFailed, testsRun);
				pauseIfTesting();
				break;
			case 1:
				functionalTestMenu();
				break;
			case 2:
				graphicalTestMenu(&g);
				break;
		}


	}

	return 0;
}

#endif
