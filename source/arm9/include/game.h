/**
 * @file game.h
 *
 * @brief The game class represents a zbe game
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

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

// On which screen should the main gameplay occur? Should probably always be &oamMain.
#define ZBE_GAMEPLAY_OAM &oamMain


#include <nds.h>
#include <stdio.h>
#include <vector>
#include <string>

// for zbeAssets
#include "assets.h"
#include "assettypes.h" // for levelAsset struct
#include "vars.h"

#include "level.h"

/**
 * game class
 *
 *
 * @author Joe Balough
 */
class game
{
public:
	/**
	 * game class constructor
	 *
	 * Initializes up the assets for this game
	 *
	 * @param char *filename
	 *   Filename to use to initialize the assets object
	 * @author Joe Balough
	 */
	game(char* filename);


	/**
	 * game class deconstructor
	 *
	 * Deletes the zbeAssets object and sets it to NULL
	 *
	 * @author Joe Balough
	 */
	~game();


	/**
	 * run function
	 *
	 * Called by the main function to play the game.
	 *
	 * @author Joe Balough
	 */
	void run();


	/**
	 * runLevel function
	 *
	 * Initializes and runs the level with the passed levelId.
	 * Used by the testing framework
	 *
	 * @param uint32 levelId
	 *   id of the level to run
	 * @author Joe Balough
	 */
	void runLevel(uint32 levelId);


	/**
	 * getLevelNames function
	 *
	 * Creates a vector of strings containing the name of every level in the game.
	 *
	 * @param vector<string> &tests
	 *   vector of strings passed by reference, This is filled with the level names.
	 * @author Joe Balough
	 */
	void getLevelNames(vector<string> &tests)
	{
		// Add every level's name to the vector
		for (unsigned int i = 0; i < zbeAssets->numLevels(); i++)
		{
			tests.push_back(zbeAssets->getLevelName(i));
		}
	}


	/**
	 * numLevels function
	 *
	 * Returns the number of levels in the game
	 *
	 * @return uint32
	 *   number of leves in the game
	 * @author Joe Balough
	 */
	inline uint32 numLevels()
	{
		return zbeAssets->numLevels();
	}


#ifdef ZBE_TESTING
	/**
	 * getDebugMessage function
	 *
	 * Returns the debugMessage for a level
	 *
	 * @param uint32
	 *   id for level
	 * @return string
	 *   level's debug message
	 * @author Joe Balough
	 */
	inline string getDebugMessage(uint32 l)
	{
		return zbeAssets->getDebugMessage(l);
	}
#endif

private:

};

#endif
