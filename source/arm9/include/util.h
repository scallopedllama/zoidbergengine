/**
 * @file util.h
 *
 * @brief Contains useful global utility functions
 *
 * @see object.h
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
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#define ZOIDBERG_USE_EXT_PAL false

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

/**
 * Global Utility Function; die
 *
 * A wrapper for any action we would want to do to stop everything.
 * This simply runs while(true); for now
 *
 * @author Joe Balough
 */
inline void die()
{
	while (true);
}

/**
 * Video Initialization routine
 *
 * Configures the DS video hardware
 *
 * @author Joe Balough
 */
void initVideo();


/**
 * pause funciton
 *
 * Stops everything and waits for the player to press any key before returning.
 *
 * @author Joe Balough
 */
void pause();

/**
 * menu function
 *
 * Displays a list of strings to the user as a menu and allows them to pick one option using
 * the up/down and B/A keys. This funciton will stop everything else
 *
 * @param vector<string> list
 *  A vector of strings to use as the list of options in the menu
 * @param string message
 *  An optional string to put above the menu list. NOTE: This string cannot be wrapped, add newlines to
 *  span multiple lines.
 * @return int
 *  Returns the index of the string the user selected
 * @author Joe Balough
 */
int menu(vector<string> list, string message = "");


/**
 * pauseIfTesting function
 * Prints "\nPress any key to continue\n", pauses, then clears the screen if
 * ZBE_TESTING defined.
 * @author Joe Balough
 */
inline void pauseIfTesting()
{
	#ifdef ZBE_TESTING
	iprintf("\nPress any key to continue\n");
	pause();
	consoleClear();
	#endif
}

#endif // UTIL_H_INCLUDED
