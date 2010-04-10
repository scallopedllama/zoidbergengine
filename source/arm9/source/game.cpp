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
