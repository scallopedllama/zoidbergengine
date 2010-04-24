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
#define ZOIDBERG_GAMEPLAY_OAM &oamMain


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


	uint32 runLevel(uint32 levelId)
	{}


	void getLevelNames(vector<string> &tests)
	{}


	uint32 numLevels()
	{}

private:

};

#endif
