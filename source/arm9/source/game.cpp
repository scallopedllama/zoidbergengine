#include "game.h"

game::game(string filename)
{
	// Open the zbe datafile
	zbeAssets = new assets(filename, ZOIDBERG_GAMEPLAY_OAM);
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
