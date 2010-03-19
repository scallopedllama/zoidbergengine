#include "level.h"
#include "physics.h"
#include <stdio.h>
using namespace decapod;

// level constructor
level::level(assets *a)
{
	// initialize the oam table
	initOAM(oam);

	// indicate that all matrices and sprites are available
	for (int i = 0; i < MATRIX_COUNT; i++)
	{
		matrixAvail[i] = true;
		spriteAvail[i] = true;
	}

	// Save the pointer to the assets
	zegAssets = a;
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
void level::addSprite(bool mkeHero, u32 tilesId, u32 palId, int x, int y, int width, int height, int angle, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, bool mosaic)
{
	// Request the assets for this sprite be loaded
	u16 tilesIndex;
	u8 palIndex;
	zegAssets->loadTiles(tilesId, tilesIndex);
	zegAssets->loadPalette(palId, palIndex);

    // Create the sprite
    int spriteIndex = getSpriteEntry();
    object *newObj;
    if (mkeHero)
		newObj = (object *) new hero(getSpriteEntry(spriteIndex), spriteIndex, x, y, width, height, blendMode, colorMode, shape, size, tilesIndex, palIndex);
    else
		newObj = new object(getSpriteEntry(spriteIndex), spriteIndex, x, y, width, height, blendMode, colorMode, shape, size, tilesIndex, palIndex);
	if(angle > 0)
	{
		int matrixIndex = getMatrix();
		newObj->makeRotateScale(matrixIndex, getMatrix(matrixIndex),angle);
	}
	newObj->setPriority(OBJPRIORITY_0);

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
		updateOAM(oam);
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
		if( !decapod::intersection( *objects[0], *objects[1] ) ) printf("\nCollision Detected!\n");
		objects[i]->update(touch);


	}
}

// initializes a local OAMTable
void level::initOAM(OAMTable &oam)
{
	// reset all the attributes
    for (int i = 0; i < SPRITE_COUNT; i++)
		clearSprite(&oam.oamBuffer[i]);

    // and the matrices (that's the identity matrix btw)
    for (int i = 0; i < MATRIX_COUNT; i++)
    {
        oam.matrixBuffer[i].hdx = 1 << 8;
        oam.matrixBuffer[i].hdy = 0;
        oam.matrixBuffer[i].vdx = 0;
        oam.matrixBuffer[i].vdy = 1 << 8;
    }
}

// clears out the attributes for a SpriteEntry
void level::clearSprite(SpriteEntry *Sprite)
{
	Sprite->attribute[0] = ATTR0_DISABLED;
	Sprite->attribute[1] = 0;
	Sprite->attribute[2] = 0;
}

// copies a local OAMTable into memory, replacing the old one.
void level::updateOAM(OAMTable &oam)
{
	// avoid any caching issues
    DC_FlushAll();
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL, oam.oamBuffer, OAM, SPRITE_COUNT * sizeof(SpriteEntry));
}
