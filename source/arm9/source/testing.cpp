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
#include <vector>
#include "game.h"
#include "util.h"

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
	 */
	virtual void run()
	{
		iprintf("Whoops! You defined your\nfunctionalTest wrong.\n");
		pause();
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
	virtual void run()
	{
		iprintf("Gonna test the collisionMatrix now\n");
		pause();
	}

private:

};







/**
 *     RUN TEST FUNCTIONS
 */

/**
 * getFunctionalTests() function
 *
 * returns a vector of functionalTests that can be run.
 * If you add a new functional test, make sure to add it to the vector here.
 *
 * @param vector<functionalTest> &tests
 *   A vector of a list of functionalTests to add the functionalTests to.
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
 * functionalTestMenu(); shows a menu to the user listing all of the functional tests that can be run.
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
		tests[choice]->run();

		// Let the user pick a different test
		choice = menu(list, "Please select a \nfunctional test to run:\n");
	}

	// Delete all the functional tests made for that vector
	for (unsigned int i = 0; i < tests.size(); i++)
	{
		delete tests[i];
	}
}


void runFunctionalTests()
{}


void runGraphicalTests()
{}


void graphicalTestMenu()
{}




/**
 *     MAIN FUNCTION
 */
int main()
{
	initVideo();

	// Initialize libfat
	fatInitDefault();

	// Print some debug information for the user
	string msg = "ZOIDBERG ENGINE TESTING BUILD\n\nPlease select a test to run:\n";

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
				runGraphicalTests();
				break;
			case 1:
				functionalTestMenu();
				break;
			case 2:
				graphicalTestMenu();
				break;
		}


	}

	// make a game
	game g((char *) "/assets.zbe");
	g.run();

	return 0;
}

#endif
