#include "game.h"

game::game(char *filename)
{
	
	
	// initialize the assets
	zbeAssets = new assets(filename, ZOIDBERG_GAMEPLAY_OAM);
	
}
