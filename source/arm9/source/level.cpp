#include "level.h"
#include "physics.h"
#include <stdio.h>
using namespace decapod;

// level constructor
level::level(char *filename)
{
	// initialize the oam table
	oam = ZOIDBERG_GAMEPLAY_OAM;
	
	// initialize the assets
	zbeAssets = new assets(filename, oam);
	
	// indicate that all matrices and sprites are available
	for (int i = 0; i < MATRIX_COUNT; i++)
	{
		matrixAvail[i] = true;
		spriteAvail[i] = true;
	}
}

// level destructor
level::~level()
{
	for(unsigned int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
}

// tries to get an affine transformation matrix for use with the rotoZoom style sprite.
// returns the matrix id or ZOIDBERG_NO_MATRICES if they're all taken
int level::getMatrix()
{
	for (unsigned int i = 0; i < MATRIX_COUNT; i++)
	{
		if (matrixAvail[i])
		{
			matrixAvail[i] = false;
			return i;
		}
	}
	// must be all taken
	return ZOIDBERG_NO_MATRICES;
}

// tries to get a spriteEntry for a constructor to use. Returns index of spriteEntry or ZOIDBERG_NO_SPRITES if it can't.
int level::getSpriteEntry()
{
	for (int i = 0; i < SPRITE_COUNT; i++)
	{
		if (spriteAvail[i])
		{
			spriteAvail[i] = false;
			return i;
		}
	}
	// must be all taken
	return ZOIDBERG_NO_SPRITES;
}

// TEMP FCTN: adds a sprite to the level
void level::addSprite(bool mkeHero, u32 gfxId, u32 palId, int x, int y)
{
	// Request the assets for this sprite be loaded
	uint16 *frame = zbeAssets->loadGfx(gfxId);
	uint8 palIndex = zbeAssets->loadPalette(palId);
	void ***gfx = NULL;
	SpriteSize size = zbeAssets->getSpriteSize(gfxId);
	int numAnim = 1; int numFrames[1]; numFrames[0] = 1;
	
	//object::object(OamState *Oam, 
	//			   int SpriteId, int PaletteId, 
	//			   void ***Gfx, int NumAnim, int NumFrames[], void *Frame,
	//			   int X, int Y, int Priority, SpriteSize Size, SpriteColorFormat ColorFormat, bool IsSizeDouble = true, bool Hidden = false,
	//			   int MatrixId = -1, int Width = 1, int Height = 1, int Angle = 0,
	//			   bool Mosaic = false);
	
	
    // Create the sprite
    int spriteIndex = getSpriteEntry();
    object *newObj;
    if (mkeHero)
		newObj = (object *) new hero(oam, spriteIndex, palIndex, gfx, numAnim, numFrames, frame, x, y, 0, size, SpriteColorFormat_16Color,
		true, false, getMatrix(), 0, 29, 455, false);
    else
		newObj = new object(oam, spriteIndex, palIndex, gfx, numAnim, numFrames, frame, x, y, 0, size, SpriteColorFormat_16Color);
	
	// Add that object to the list
	objects.push_back(newObj);
}

// The 'main game loop' for this level
void level::run()
{
	// start running the main loop
	while(true)
	{
		update();
		swiWaitForVBlank();
		oamUpdate(oam);
	}
}

// updates all objects on this level
void level::update()
{
	// update the button pressage and touch screen data
	scanKeys();
	touchPosition *touch = NULL;
	touchRead(touch);

	//iterate through all the objects in that list
	for(unsigned int i=0; i<objects.size(); i++)
	{
		// run their respective update functions
		//if( !decapod::intersection( *objects[0], *objects[1] ) ) printf("\nCollision Detected!\n");
		objects[i]->update(touch);


	}
}
