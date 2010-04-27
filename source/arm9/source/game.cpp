#include "game.h"

// Constructor
game::game(char* filename)
{
	// Create the assets
	zbeAssets = new assets(filename, ZBE_GAMEPLAY_OAM);
}


// Deconstructor
game::~game()
{
	delete zbeAssets;
	zbeAssets = NULL;
}


// Runs the game
void game::run()
{
	// Load up the first level
	levelAsset *firstLvlAsset = zbeAssets->loadLevel(0);

	// Make that level
	level first(firstLvlAsset, ZBE_GAMEPLAY_OAM);

	// Run it
	first.run();
}


// Runs a level of the game
void game::runLevel(uint32 levelId)
{
	//Load up the desired level
	levelAsset *thisLvlAsset = zbeAssets->loadLevel(levelId);

	// Make it
	level thisLevel(thisLvlAsset, ZBE_GAMEPLAY_OAM);

	//Run
	thisLevel.run();
}



