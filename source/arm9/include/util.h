/**
 * @file util.h
 *
 * @brief Contains useful global utility functions
 *
 * @see object.h
 * @author Joe Balough
 */

/*
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

#define ZBE_USE_EXT_PAL false

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
 * libnds API calls:
 *   powerOn(POWER_ALL_2D) -- Enables 2D graphics on the DS
 *   lcdMainOnTop() -- Tells the DS to use the top screen as the main screen
 *   vramSetMainBanks -- Tells the DS how to map the video ram banks.
 *               We use A for the main screen's backgrounds
 *                      B for the main screen's sprites
 *                      C for the sub screen's backgrounds
 *                      D for the sub screen's sprites
 *   videoSetMode -- Tells the DS what graphics features to use on the main screen
 *               We set the graphics mode to mode 5 (with affine transformations)
 *                  enable 4 backgrounds on the main screen
 *                         tiled sprites on the main screen
 *   videoSetModeSub -- Tells the DS what graphics features to use on the sub screen
 *               We set the graphics to mode 5 and enable 1 background
 *   oamInit -- initialize the main and sub OAMs for displaying sprites
 *   consoleDemoInit -- enable console output using printf on the sub screen
 *
 * @author Joe Balough
 */
void initVideo();


/**
 * pause funciton
 *
 * Stops everything and waits for the player to press any key before returning.
 *
 * libnds API calls:
 *   scanKeys -- check for keys that have been pressed
 *   keysUp -- returns all the buttons that have been released
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
 * libnds API calls:
 *   consoleClear -- clears the console text
 *   scanKeys -- check for keys that have been pressed
 *   keysUp -- returns all the buttons that have been released
 *   KEY_* -- define to determine if the * button has been pressed
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
 * menu wrapper function, yesNoMenu
 *
 * Makes a menu of "Yes" and "No" and returns the response
 *
 * @param string message
 *   The message to display with the menu
 * @return bool
 *   Whether the user answered yes or no
 * @author Joe Balough
 */
inline bool yesNoMenu(string message)
{
	// Make the menu
	vector<string> list;
	list.push_back("Yes");
	list.push_back("No");

	// Run it, return true if the user picked the first option.
	if (menu(list, message) == 0)
		return true;
	else
		return false;
}


/**
 * pauseIfTesting function
 *
 * Prints "\nPress any key to continue\n", pauses, then clears the screen if
 * ZBE_TESTING defined.
 *
 * libnds API calls:
 *   consoleClear -- clears the console text
 *
 * @author Joe Balough
 */
inline void pauseIfTesting()
{
	#ifdef ZBE_TESTING
	iprintf("\nPress any button to continue\n");
	pause();
	consoleClear();
	#endif
}

#endif // UTIL_H_INCLUDED
