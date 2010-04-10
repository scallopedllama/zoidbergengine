#include "game.h"

game::game(char *filename)
{
	// Open the zbe datafile
	zbeFile = fopen(filename, "rb");
	if(zbeData == NULL)
	{
		iprintf("Fail\n");
	}
	else
	{
		iprintf("Success\n");
		// initialize the assets
		zbeAssets = new assets(zbeFile, ZOIDBERG_GAMEPLAY_OAM);
	}
}

game::~game()
{
	delete zbeAssets;
	zbeAssets = NULL;
}

void game::run()
{
	// Load up the first level
	levelAssets *firstAssets = zbeAssets->loadLevel(0);
	
	// Make that level
	level first(firstAssets, ZOIDBERG_GAMEPLAY_OAM);
}
