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
	// TODO: actually handle the return values on all these file functions. 
	//       They're all ignored so there is not fault tolerance.

	// Get the version number out of the zeg file
	uint16 version = fread16(zbeData);
	iprintf("v %d\n", version);

	// Get the total number of assets
	uint32 numAssets = fread32(zbeData);
	iprintf("#assets %d\n", numAssets);

	// Get the number of tiles
	uint32 numTiles = fread32(zbeData);
	iprintf("#gfx %d\n", numTiles);

	// Get all of the gfx tiles
	for (uint32 i = 0; i < numTiles; i++)
	{
		// Make a new assetStatus for the vector
		assetStatus newAsset;
		
		// The very first byte of data in gfx tiles is its size
		uint8 size = fread8(zbeData);
		switch (size)
		{
			case 8:
				newAsset.size = OBJSIZE_8;
				iprintf(" %d's size 8x8\n", i);
				break;
			case 16:
				newAsset.size = OBJSIZE_16;
				iprintf(" %d's size 16x16\n", i);
				break;
			case 32:
				newAsset.size = OBJSIZE_32;
				iprintf(" %d's size 32x32\n", i);
				break;
			case 64:
				newAsset.size = OBJSIZE_64;
				iprintf(" %d's size 64x64\n", i);
		}
		
		// Get the length of this gfx tiles
		newAsset.length = fread16(zbeData);
		iprintf(" len %d\n", newAsset.length);

		// Add the current position in the file to the vector
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		newAsset.position = curPos;

		// Set that it is not loaded, assign a value to the union
		// to get it using the proper value
		newAsset.loaded = false;
		newAsset.index.index16 = 0;
		
		// Push this asset on the array
		gfxStatus.push_back(newAsset);

		// Seek past this object
		fseek(zbeData, newAsset.length, SEEK_CUR);
	}

	// Get the number of palettes
	uint32 numPals = fread32(zbeData);
	iprintf("#pals %d\n", numPals);

	// Get all of the palettes
	for (uint32 i = 0; i < numPals; i++)
	{
		// Make a new status for this palette
		assetStatus newAsset;
		
		// Get the length of this palette
		newAsset.length = fread16(zbeData);
		iprintf(" %d's len %d\n", i, newAsset.length);

		// Add the current position in the file to the vector
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		newAsset.position = curPos;

		// Initialize the index so the union is set up
		// Then mark that it isn't loaded
		newAsset.index.index8 = 0;
		newAsset.loaded=false;
		
		// Push the new asset on to the array
		palStatus.push_back(newAsset);

		// Seek past this object
		fseek(zbeData, newAsset.length, SEEK_CUR);
	}
}

// Reads a 32 bit integer from the input file
uint32 assets::fread32(FILE *input)
{
	uint32 variable;
	fread(&variable, sizeof(uint32), 1, input);
	return variable;
}

// Reads a 16 bit integer from the input file
uint16 assets::fread16(FILE *input)
{
	uint16 variable;
	fread(&variable, sizeof(uint16), 1, input);
	return variable;
}

// Reads an 8 bit integer from the input file
uint8 assets::fread8(FILE *input)
{
	uint8 variable;
	fread(&variable, sizeof(uint8), 1, input);
	return variable;
}

// Loads tiles with id into memory
void assets::loadGfx(u32 id, u16 &tilesIndex)
{
	iprintf("tile[%d] requested\n", id);

	// See if it's already loaded
	if (gfxStatus[id].loaded)
	{
		iprintf(" cache hit->%d\n", gfxStatus[id].index.index16);

		// Already loaded so set the index and return
		tilesIndex = gfxStatus[id].index.index16;
		return;
	}

	iprintf(" cache miss\n");

	// Need to load it from disk into memory
	// Seek to the proper place in the file
	fsetpos(zbeData, &gfxStatus[id].position);

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
	uint16 length = gfxStatus[id].length;
	void *data = malloc(length * sizeof(u8));
	if (fread(data, sizeof(u8), length, zbeData) < length)
		iprintf(" data load error\n");
	dmaCopyHalfWords(3, data, &SPRITE_GFX[curIndex * OFFSET_MULTIPLIER], length);
	free(data);

	// Update some variables
	gfxStatus[id].loaded = true;
	gfxStatus[id].index.index16 = curIndex;
	tilesIndex = curIndex;

	iprintf(" loaded->%d\n", tilesIndex);

	// Update the index for the next call
	curIndex += length / BYTES_PER_16_COLOR_TILE;
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
	fsetpos(zbeData, &palStatus[id].position);

	// Set some variables
	static int curIndex = 0;
    static const int COLORS_PER_PALETTE = 16;

	// Start copying
	uint16 length = palStatus[id].length;
	void *data = malloc(length * sizeof(u8));
	if (fread(data, sizeof(u8), length, zbeData) < length)
		iprintf(" data load error\n");
	dmaCopyHalfWords(3, data, &SPRITE_PALETTE[curIndex * COLORS_PER_PALETTE], length);
	free(data);

	// Update some variables
	palStatus[id].loaded = true;
	palStatus[id].index.index8 = curIndex;
	palIndex = curIndex;

	iprintf(" loaded->%d\n", palIndex);

	// Update the index for the next call
	curIndex++;
}
