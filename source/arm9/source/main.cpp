/*
 *  main.cpp
 *
 *  Created by Jaeden Amero on 11/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include <nds.h>
#include <maxmod9.h>
#include <fat.h>
#include <assert.h>
#include <stdio.h>

#include "game.h"


void initVideo()
{
	powerOn(POWER_ALL_2D);
	lcdMainOnBottom();

    vramSetMainBanks(VRAM_A_MAIN_SPRITE,
                     VRAM_B_MAIN_BG_0x06020000,
                     VRAM_C_SUB_BG_0x06200000,
                     VRAM_D_SUB_SPRITE);


    //  Set the video mode on the main screen.
    videoSetMode(MODE_5_2D | // Set the graphics mode to Mode 5
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
