#include "util.h"

// Initialize the video
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
