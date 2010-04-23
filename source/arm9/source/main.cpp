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

#include <nds.h>
#include <maxmod9.h>
#include <fat.h>
#include <assert.h>
#include <stdio.h>
#include "game.h"

#define ZOIDBERG_USE_EXT_PAL false


void initVideo()
{
	powerOn(POWER_ALL_2D);
	lcdMainOnBottom();

	vramSetMainBanks(VRAM_A_MAIN_BG,
					VRAM_B_MAIN_SPRITE_0x06400000,
					VRAM_C_SUB_BG_0x06200000,
					VRAM_D_SUB_SPRITE
					);

	//  Set the video mode on the main screen.
	videoSetMode(MODE_0_2D | // Set the graphics mode to Mode 5
			DISPLAY_BG0_ACTIVE | // Enable BG0 for display
			DISPLAY_BG1_ACTIVE | // Enable BG1 for display
			DISPLAY_BG2_ACTIVE | // Enable BG2 for display
			DISPLAY_BG3_ACTIVE | // Enable BG3 for display
			DISPLAY_SPR_ACTIVE | // Enable sprites for display
			DISPLAY_SPR_1D       // Enable 1D tiled sprites
			);

	//  Set the video mode on the sub screen.
	videoSetModeSub(MODE_5_2D | // Set the graphics mode to Mode 5
			   DISPLAY_BG1_ACTIVE); // Enable BG1 for display of console output

	oamInit(&oamMain, SpriteMapping_1D_32, ZOIDBERG_USE_EXT_PAL);
	oamInit(&oamSub, SpriteMapping_1D_32, ZOIDBERG_USE_EXT_PAL);

	consoleDemoInit();
}

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
