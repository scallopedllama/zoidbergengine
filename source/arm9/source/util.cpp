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


// Pause gameplay until player presses a key
void pause()
{
	scanKeys();
	while (!keysUp())
	{
		scanKeys();
	}
}

// display a list of strings for the user to choose from
int menu(vector<string> list)
{
	int selected = 0;
	
	consoleClear();
	for (unsigned int i = 0; i < list.size(); i++)
	{
		// Show a star if the user has this string selected
		if (selected == i) iprintf(" * ");
		else               iprintf("   ");
		
		// Show this message
		iprintf("%s\n", list[i].c_str());
	}
	
	// Wait until the user presses a button
	while (true)
	{
		// Update the keys
		scanKeys();
		
		// See if the user has pressed any keys
		if (keysUp() & KEY_UP)
		{
			// Erase old one
			iprintf("\x1b[%d;1H ", selected);
			
			// Updated selected
			--selected;
			if(selected < 0) selected = list.size() - 1;
			
			// Update cursor
			iprintf("\x1b[%d;1H*", selected);
		}
		else if (keysUp() & KEY_DOWN)
		{
			// Erase old one
			iprintf("\x1b[%d;1H ", selected);
			
			// Updated selected
			selected = (selected + 1) % list.size();
			
			// Update cursor
			iprintf("\x1b[%d;1H*", selected);
		}
		else if (keysUp() & KEY_A || keysUp() & KEY_B)
			break;
		
	}
	
	consoleClear();
	return selected;
}

