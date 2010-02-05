#include "level.h"

level::level()
{
	//initialize the oam table
	oam = new OAMTable();
	initOAM(oam);

	//indicate that all matrices and sprites are available
	for (int i=0; i<MATRIX_COUNT; i++)
	{
		matrixAvail[i] = true;
		spriteAvail[i] = true;
	}

	//load up all the necessary assets n stuff
}

level::~level()
{
	delete oam;
}

//tries to get an affine transformation matrix for use with the rotoZoom style sprite.
//returns the matrix id or -1 if they're all taken
int level::getMatrix()
{
	for (int i=0; i<MATRIX_COUNT; i++)
	{
		if (matrixAvail[i])
		{
			matrixAvail[i] = false;
			return i;
		}
	}
	//must be all taken
	return -1;
}

//tries to get a spriteEntry for a constructor to use. Returns index of spriteEntry or -1 if it can't.
int level::getSpriteEntry()
{
	for (int i=0; i<SPRITE_COUNT; i++)
	{
		if (spriteAvail[i])
		{
			spriteAvail[i] = false;
			return i;
		}
	}
	//must be all taken
	return -1;
}

void level::addSprite(const void *tiles, u32 tilesLen, const void *palette, u32 paletteLen, int x, int y, int width, int height, int angle, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, bool mosaic)
{
    /*  Define some sprite configuration specific constants.
     *
     *  We will use these to compute the proper index into memory for certain
     *  tiles or palettes.
     *
     *  OFFSET_MULTIPLIER is calculated based on the following formula from
     *  GBATEK (http://nocash.emubase.de/gbatek.htm#dsvideoobjs):
     *      TileVramAddress = TileNumber * BoundaryValue
     *  Since SPRITE_GFX is a uint16*, the compiler will increment the address
     *  it points to by 2 for each change in 1 of the array index into
     *  SPRITE_GFX. (The compiler does pointer arithmetic.)
     */
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                           * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof(SPRITE_GFX[0]);

    // Keep track of the gfx index and palette index
    static int gfxIndex = 0;
    static int palIndex = 0;

    // Create the sprite and make it a rotozoomer
    int spriteIndex = getSpriteEntry();
	object *newSprite = new object(getSpriteEntry(spriteIndex), spriteIndex, x, y, width, height, blendMode, colorMode, shape, size, gfxIndex, palIndex);
	if(angle > 0)
	{
		int matrixIndex = getMatrix();
		newSprite->makeRotateScale(matrixIndex, angle, getMatrix(matrixIndex));
	}
	newSprite->setPriority(OBJPRIORITY_0);

    // Copy palette
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL, palette, &SPRITE_PALETTE[palIndex * COLORS_PER_PALETTE], paletteLen);
    // Copy sprite
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL, tiles, &SPRITE_GFX[gfxIndex * OFFSET_MULTIPLIER], tilesLen);

	//increment starting index for the next sprite
	gfxIndex += tilesLen / BYTES_PER_16_COLOR_TILE;
	palIndex++;
}

void level::run()
{
	//start running the main loop
	while(true)
	{
		swiWaitForVBlank();
		updateOAM(oam);
	}
}

void level::initOAM(OAMTable * oam) {
	//reset all the attributes
    for (int i = 0; i < SPRITE_COUNT; i++)
		clearSprite(&oam->oamBuffer[i]);

    //and the matrices (that's the identity matrix btw)
    for (int i = 0; i < MATRIX_COUNT; i++)
    {
        oam->matrixBuffer[i].hdx = 1 << 8;
        oam->matrixBuffer[i].hdy = 0;
        oam->matrixBuffer[i].vdx = 0;
        oam->matrixBuffer[i].vdy = 1 << 8;
    }
}

void level::clearSprite(SpriteEntry *Sprite)
{
	Sprite->attribute[0] = ATTR0_DISABLED;
	Sprite->attribute[1] = 0;
	Sprite->attribute[2] = 0;
}

void level::updateOAM(OAMTable * oam) {
	//avoid any caching issues
    DC_FlushAll();
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, oam->oamBuffer, OAM, SPRITE_COUNT * sizeof(SpriteEntry) );
}
