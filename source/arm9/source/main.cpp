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

#include "level.h" //initOAM updateOAM

/* Backgrounds */
#include "starField.h"
#include "planet.h"
#include "splash.h"
/* Sprites */
#include "orangeShuttle.h"
#include "moon.h"
/* Sounds */
#include "soundbank.h"
#include "soundbank_bin.h"

/* Select a low priority DMA channel to perform our background copying. */
static const int DMA_CHANNEL = 3;

void initVideo() {
	//  Turn on the 2D graphics core.
	powerOn(POWER_ALL_2D);
	lcdMainOnBottom();

    /*
     *  Map VRAM to display a background on the main and sub screens.
     *
     *  The vramSetMainBanks function takes four arguments, one for each of the
     *  major VRAM banks. We can use it as shorthand for assigning values to
     *  each of the VRAM bank's control registers.
     *
     *  We map banks A and B to main screen  background memory. This gives us
     *  256KB, which is a healthy amount for 16-bit graphics.
     *
     *  We map bank C to sub screen background memory.
     *
     *  We map bank D to LCD. This setting is generally used for when we aren't
     *  using a particular bank.
     *
     *  We map bank E to main screen sprite memory (aka object memory).
     */
    vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000,
                     VRAM_B_MAIN_BG_0x06020000,
                     VRAM_C_SUB_BG_0x06200000,
                     VRAM_D_LCD);

    vramSetBankE(VRAM_E_MAIN_SPRITE);

    /*  Set the video mode on the main screen. */
    videoSetMode(MODE_5_2D | // Set the graphics mode to Mode 5
                 DISPLAY_BG2_ACTIVE | // Enable BG2 for display
                 DISPLAY_BG3_ACTIVE | // Enable BG3 for display
                 DISPLAY_SPR_ACTIVE | // Enable sprites for display
                 DISPLAY_SPR_1D       // Enable 1D tiled sprites
                 );

    /*  Set the video mode on the sub screen. */
    videoSetModeSub(MODE_5_2D | // Set the graphics mode to Mode 5
                   DISPLAY_BG1_ACTIVE); // Enable BG1 for display of console output

    consoleDemoInit();
}

void initBackgrounds() {
    /*  Set up affine background 3 on main as a 16-bit color background. */
    REG_BG3CNT = BG_BMP16_256x256 |
                 BG_BMP_BASE(0) | // The starting place in memory
                 BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    REG_BG3PA = 1 << 8;
    REG_BG3PB = 0;
    REG_BG3PC = 0;
    REG_BG3PD = 1 << 8;

    /*  Place main screen background 3 at the origin (upper left of the
     *  screen).
     */
    REG_BG3X = 0;
    REG_BG3Y = 0;

    /*  Set up affine background 2 on main as a 16-bit color background. */
    REG_BG2CNT = BG_BMP16_128x128 |
                 BG_BMP_BASE(8) | // The starting place in memory
                 BG_PRIORITY(2);  // A higher priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 1 << 8;

    /*  Place main screen background 2 in an interesting place. */
    REG_BG2X = -(SCREEN_WIDTH / 2 - 32) << 8;
    REG_BG2Y = -32 << 8;

    /*  Set up affine background 3 on the sub screen as a 16-bit color
     *  background.
     */
    REG_BG3CNT_SUB = BG_BMP16_256x256 |
                     BG_BMP_BASE(0) | // The starting place in memory
                     BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the sub screen background 3
     *  to be the identity matrix.
     */
    REG_BG3PA_SUB = 1 << 8;
    REG_BG3PB_SUB = 0;
    REG_BG3PC_SUB = 0;
    REG_BG3PD_SUB = 1 << 8;

    /*
     *  Place main screen background 3 at the origin (upper left of the screen)
     */
    REG_BG3X_SUB = 0;
    REG_BG3Y_SUB = 0;
}

void displayStarField() {
    dmaCopyHalfWords(DMA_CHANNEL,
                     starFieldBitmap, /* This variable is generated for us by
                                       * grit. */
                     (uint16 *)BG_BMP_RAM(0), /* Our address for main
                                               * background 3 */
                     starFieldBitmapLen);
}

void displayPlanet() {
    dmaCopyHalfWords(DMA_CHANNEL,
                     planetBitmap, /* This variable is generated for us by
                                    * grit. */
                     (uint16 *)BG_BMP_RAM(8), /* Our address for main
                                               * background 2 */
                     planetBitmapLen);
}

void displaySplash() {
    dmaCopyHalfWords(DMA_CHANNEL,
                     splashBitmap, /* This variable is generated for us by
                                    * grit. */
                     (uint16 *)BG_BMP_RAM_SUB(0), /* Our address for sub
                                                     background 3 */
                     splashBitmapLen);
}


int main() {
	initVideo();
	initBackgrounds();

	// Initialize maxmod using the memory based soundbank set up.
	mmInitDefaultMem((mm_addr)soundbank_bin);

	// Initialize libfat
	fatInitDefault();

	// Display the backgrounds
	displayStarField();
	displayPlanet();
	//displaySplash();

	iprintf("test\nfoobar!\n");

	//make a level
	level *lvl = new level();
	//void addSprite(const void *tiles, u32 tilesLen, const void *palette, u32 paletteLen, int x, int y, int width, int height, int angle, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, bool mosaic)
	lvl->addSprite(true, orangeShuttleTiles, orangeShuttleTilesLen, orangeShuttlePal, orangeShuttlePalLen, 25, 45, 32, 32, 29568, OBJMODE_NORMAL, OBJCOLOR_16, OBJSHAPE_SQUARE, OBJSIZE_64);
	lvl->addSprite(false, moonTiles, moonTilesLen, moonPal, moonPalLen, 25, 40, 32, 32, 0, OBJMODE_NORMAL, OBJCOLOR_16, OBJSHAPE_SQUARE, OBJSIZE_32);
	lvl->run();

	return 0;
}
