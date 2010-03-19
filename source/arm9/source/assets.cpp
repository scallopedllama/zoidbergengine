#include "assets.h"

assets::assets(char *filename)
{
	// Load the file
	iprintf("Opening %s\n", filename);
	zbeData = fopen(filename, "rb");
	if(zbeData == NULL)
	{
		iprintf("Fail\n");
	}
	else
	{
		iprintf("Success\n");
		parseZbe();
	}
}

void assets::parseZbe()
{
	// TODO: actually handle the return values on all these file functions. They're all ignored so there is not fault tolerance.

	// Get the version number out of the zeg file
	uint16 version = 0;
	fread16(zbeData, version);
	iprintf("v %d\n", version);

	// Get the total number of assets
	uint32 numAssets = 0;
	fread32(zbeData, numAssets);
	iprintf("#assets %d\n", numAssets);

	// Get the number of tiles
	uint32 numTiles = 0;
	fread32(zbeData, numTiles);
	iprintf("#gfx %d\n", numTiles);

	// Get all of the tiles
	for (uint32 i = 0; i < numTiles; i++)
	{
		// Get the length of this tile
		uint16 thisLen = 0;
		fread16(zbeData, thisLen);
		iprintf(" %d's len %d\n", i, thisLen);

		// Store it on the end of the vector
		tileLen.push_back(thisLen);

		// Add the current position in the file to the vector
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		tilePos.push_back(curPos);

		// Add a new status to the end of the tileStatus vector
		// Don't forget to set the status appropriately
		assetStatus thisStatus;
		thisStatus.index.index16 = ZOIDBERG_ASSET_NOT_LOADED;
		tileStatus.push_back(thisStatus);

		// Seek past this object
		fseek(zbeData, thisLen, SEEK_CUR);
	}

	// Get the number of palettes
	uint32 numPals = 0;
	fread32(zbeData, numPals);
	iprintf("#pals %d\n", numPals);

	// Get all of the palettes
	for (uint32 i = 0; i < numPals; i++)
	{
		// Get the length of this palette
		uint16 thisLen = 0;
		fread16(zbeData, thisLen);
		iprintf(" %d's len %d\n", i, thisLen);

		// Store it on the end of the vector
		palLen.push_back(thisLen);

		// Add the current position in the file to the vector
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		palPos.push_back(curPos);

		// Add a new status to the end of the palStatus vector
		// Don't forget to set the status appropriately
		assetStatus thisStatus;
		thisStatus.index.index8 = ZOIDBERG_ASSET_NOT_LOADED;
		palStatus.push_back(thisStatus);

		// Seek past this object
		fseek(zbeData, thisLen, SEEK_CUR);
	}
}

// Reads a 32 bit integer from the input file
void assets::fread32(FILE *input, uint32 &variable)
{
	fread(&variable, sizeof(uint32), 1, input);
}

// Reads a 16 bit integer from the input file
void assets::fread16(FILE *input, uint16 &variable)
{
	fread(&variable, sizeof(uint16), 1, input);
}

// Loads tiles with id into memory
void assets::loadTiles(u32 id, u16 &tilesIndex)
{
	iprintf("tile[%d] requested\n", id);

	// See if it's already loaded
	if (tileStatus[id].loaded)
	{
		iprintf(" cache hit->%d\n", tileStatus[id].index.index16);

		// Already loaded so set the index and return
		tilesIndex = tileStatus[id].index.index16;
		return;
	}

	iprintf(" cache miss\n");

	// Need to load it from disk into memory
	// Seek to the proper place in the file
	fsetpos(zbeData, &tilePos[id]);

	// Set some variables
	static int curIndex = 0;
	/*
	 *  OFFSET_MULTIPLIER is calculated based on the following formula from
     *  GBATEK (http://nocash.emubase.de/gbatek.htm#dsvideoobjs):
     *      TileVramAddress = TileNumber * BoundaryValue
     */
    // This is the default boundary value (can be set in REG_DISPCNT)
    static const int BOUNDARY_VALUE = 32;
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof(SPRITE_GFX[0]);
    static const int BYTES_PER_16_COLOR_TILE = 32;

	// Start copying
	void *data = malloc(tileLen[id] * sizeof(u8));
	if (fread(data, sizeof(u8), tileLen[id], zbeData) < tileLen[id])
		iprintf(" data load error\n");
	dmaCopyHalfWords(3, data, &SPRITE_GFX[curIndex * OFFSET_MULTIPLIER], tileLen[id]);
	free(data);

	// Update some variables
	tileStatus[id].loaded = true;
	tileStatus[id].index.index16 = curIndex;
	tilesIndex = curIndex;

	iprintf(" loaded->%d\n", tilesIndex);

	// Update the index for the next call
	curIndex += tileLen[id] / BYTES_PER_16_COLOR_TILE;
}

// Loads palette with id into memory
void assets::loadPalette(u32 id, u8 &palIndex)
{
	iprintf("palette[%d] requested\n", id);

	// See if it's already loaded
	if (palStatus[id].loaded)
	{
		iprintf(" cache hit->%d\n", palStatus[id].index.index8);

		// Already loaded so set the index and return
		palIndex = palStatus[id].index.index8;
		return;
	}

	iprintf(" cache miss\n");

	// Need to load it from disk into memory
	// Seek to the proper place in the file
	fsetpos(zbeData, &palPos[id]);

	// Set some variables
	static int curIndex = 0;
    static const int COLORS_PER_PALETTE = 16;

	// Start copying
	void *data = malloc(palLen[id] * sizeof(u8));
	if (fread(data, sizeof(u8), palLen[id], zbeData) < palLen[id])
		iprintf(" data load error\n");
	dmaCopyHalfWords(3, data, &SPRITE_PALETTE[curIndex * COLORS_PER_PALETTE], palLen[id]);
	free(data);

	// Update some variables
	palStatus[id].loaded = true;
	palStatus[id].index.index8 = curIndex;
	palIndex = curIndex;

	iprintf(" loaded->%d\n", palIndex);

	// Update the index for the next call
	curIndex++;
}
