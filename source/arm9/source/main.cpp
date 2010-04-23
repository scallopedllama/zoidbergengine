/**
 * @file main.cpp
 *
 * @brief Initializes the video stuff and runs the game
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

#ifndef ZBE_TESTING

#include <nds.h>
#include <maxmod9.h>
#include <fat.h>
#include <assert.h>
#include <stdio.h>
#include "game.h"

int main()
{
	initVideo();

	// Initialize libfat
	fatInitDefault();

	// make a game
	game g((char *) "/assets.zbe");
	g.run();

	return 0;
}

#endif
