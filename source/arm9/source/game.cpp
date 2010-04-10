#include "game.h"

game::game(char *filename)
{
	// Open the zbe datafile
	iprintf("Opening %s\n", filename);
	zbeFile = fopen(filename, "rb");
	if(zbeFile == NULL)
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
	levelAsset *firstLvlAsset = zbeAssets->loadLevel(0);
	
	// Make that level
	level first(firstLvlAsset, ZOIDBERG_GAMEPLAY_OAM);
	
	// Run it
	first.run();
}
