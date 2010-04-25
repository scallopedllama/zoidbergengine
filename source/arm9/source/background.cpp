#include "background.h"

// loads up a background
background::background(levelBackgroundAsset *metadata, gfxAsset *tileset, uint8 paletteOffset)
{
	distance = metadata->distance;
	layer = metadata->layer;

	// Load up the backgroundAsset to get the map data
	zbeAssets->loadBackground(metadata);
	bg = metadata->background;

	// Make sure tiles data is loaded into main memory
	zbeAssets->loadGfx(tileset);

	// Init the background
	// mapBases are 2KB, tileBases are 16KB, and they overlap.
	// Map tiles occupy background->tileset->length B / 1024 (B / KB) / 2 (KB / offset) mapBases
	//   that needs to be rounded up, though so see if it fits perfectly, if it doesn't add one
	int tileSize = (tileset->length % 2048 == 0) ? tileset->length / 2048 : tileset->length / 2048 + 1;

	// Map data will ALWAYS occupy (512 px / 8 (px / tile)) * (256 px / 8 (px/tile)) * 2 (B / tile) / 1024 (B / KB) = 4 KB = 2 mapBases
	int mapBase = tileSize + (layer * 2);

	// bgId = bgInit(layer, bgType, bgSize, mapBase, tileBase)
	backgroundId = bgInit(layer, BgType_Text4bpp,  ZBE_BACKGROUND_SIZE, mapBase, 0);
	// Need to reverse the layer value to get the proper priority
	bgSetPriority(backgroundId, 3 - layer);
	iprintf(" Init'd, id %d, mb %d, ts %d\n", backgroundId, mapBase, tileSize);

	// Copy tiles into video memory (If not already loaded)
	if (!tileset->vmLoaded)
	{
		iprintf("  ld %dB, cp tileset (%x)\n", tileset->length, (unsigned int) bgGetGfxPtr(backgroundId));
		DC_FlushRange(tileset->data, tileset->length);
		dmaCopy(tileset->data, bgGetGfxPtr(backgroundId), tileset->length);
		tileset->vmLoaded = true;
		// unload the tileset (doesn't need to stay in main memory
		zbeAssets->freeGfx(tileset);
	}
	else
		iprintf("  tileset already loaded\n");

	// Copy the visible part of the background into video memory
	DC_FlushRange(bg->data, bg->length);
	mapPtr = bgGetMapPtr(backgroundId);
	iprintf("  load map -> %x\n", (int) mapPtr);

	// Row Major Order
	// We need to copy enough tiles to fill the height of the screen plus some buffer
	for (uint32 y = 0; y < ZBE_BACKGROUND_TILE_HEIGHT; y++)
	{
		for (uint32 x = 0; x < ZBE_BACKGROUND_TILE_WIDTH; x++)
		{
			// Copy the tile
			if (x < bg->w && y < bg->h)
				copyTile(x, y, x, y);
		}
	}
	iprintf(" copied map\n");

	// Load, then copy all the palettes into memory
	for (uint8 i = 0; i < metadata->palettes.size(); i++)
	{
		iprintf(" load pal\n ");

		// Load up the palette
		paletteAsset *pal = metadata->palettes[i];
		zbeAssets->loadPalette(pal);

		// Copy the palette into memory, remember to offset it from the last one
		iprintf(" and copy %dB (%x)\n", pal->length, (unsigned int) BG_PALETTE +  (i + paletteOffset) * 16);
		DC_FlushRange(pal->data, pal->length);
		dmaCopy(pal->data, BG_PALETTE + (i + paletteOffset) * 16, pal->length);
	}
}


// Updates the scroll position of this background
void background::update()
{
	// The last position of the screen in the level
	static vector2D<float> lastLevelPosition(0.0, 0.0);

	// The last position of the viewport in the Background
	static vector2D<float> lastBgPosition(0.0, 0.0);

	// Scroll the screen to to screenOffset / distance if layer < 3
	vector2D<float> thisPosition(0.0, 0.0);
	if (layer < 3)
		thisPosition = vector2D<float>(screenOffset.x / float(distance) / 8.0, screenOffset.y / float(distance) / 8.0);
	//                         screenOffset * distance if layer == 3
	if (layer == 3)
		thisPosition = vector2D<float>(screenOffset.x * float(distance) / 8.0, screenOffset.y / float(distance) / 8.0);

	consoleClear();
	printf("l %d, so(%f, %f)\n", layer, screenOffset.x, screenOffset.y);
	printf("tp (%f, %f)\n", thisPosition.x, thisPosition.y);

	// Clamp the screen Position to the extremes of the screen
	if (thisPosition.x > float(bg->w * 8) - float(SCREEN_WIDTH))  thisPosition.x = float(bg->w * 8) - float(SCREEN_WIDTH);
	if (thisPosition.y > float(bg->h * 8) - float(SCREEN_HEIGHT)) thisPosition.y = float(bg->h * 8) - float(SCREEN_HEIGHT);
	if (thisPosition.x < 0.0) thisPosition.x = 0.0;
	if (thisPosition.y < 0.0) thisPosition.y = 0.0;

	// Displacement vector
	vector2D<float> displacement(thisPosition.x - lastLevelPosition.x, thisPosition.y - lastLevelPosition.y);

	// Where the viewport should now be in the background
	vector2D<float> thisBgPosition(lastBgPosition.x + displacement.x, lastBgPosition.y + displacement.y);

	printf("llp (%f, %f),\nlbp (%f, %f)\n", lastLevelPosition.x, lastLevelPosition.y, lastBgPosition.x, lastBgPosition.y);
	printf("tp (%f, %f),\nd(%f, %f),\ntbp (%f, %f)\n", thisPosition.x, thisPosition.y, displacement.x, displacement.y, thisBgPosition.x, thisBgPosition.y);

	// How many rows and columns need to be replaced
	int repRows = abs(int(displacement.y));
	int repCols = abs(int(displacement.x));

	// Find the x and y of row and column to replace by subtracting half the difference of the screen
	// dimension and the background dimension then modding the result by the background dimension (to keep it in bounds)
	//int repX = (int(thisBgPosition.x) - ( (int(ZBE_BACKGROUND_TILE_WIDTH) - (int(SCREEN_WIDTH) / 8)) / 2) ) % int(ZBE_BACKGROUND_TILE_WIDTH);
	//int repY = (int(thisBgPosition.y) - ( (int(ZBE_BACKGROUND_TILE_HEIGHT) - (int(SCREEN_HEIGHT) / 8)) / 2) ) % int(ZBE_BACKGROUND_TILE_HEIGHT);
	int repX = (int(thisBgPosition.x) - 128) % 512;
	int repY = (int(thisBgPosition.y) - 32) % 256;
	repX /= 8;
	repY /= 8;
	if (repX < 0) repX += ZBE_BACKGROUND_TILE_WIDTH;
	if (repY < 0) repY += ZBE_BACKGROUND_TILE_HEIGHT;

	iprintf("Rep %d cols at %d\n", repCols, repX);
	iprintf("Rep %d rows at %d\n", repRows, repY);
	die();

	// Replace those rows and columns
	for (int y = 0; y < repRows; y++)
	{
		for (int x = 0; x < ZBE_BACKGROUND_TILE_WIDTH; x++)
		{
			copyTile(x, y, x, repY + y);
		}
	}
	for (int x = repX; x < repX + repCols; x++)
	{
		for (int y = 0; y < ZBE_BACKGROUND_TILE_HEIGHT; y++)
		{
			copyTile(x, y, repX + x, y);
		}
	}
	die();
	// Finally, move the background and reset the lastPosition variable
	bgScroll(backgroundId, thisPosition.x, thisPosition.y);
	lastLevelPosition = thisPosition;
}


void background::copyTile(uint32 x, uint32 y, uint32 mx, uint32 my)
{
	// TODO: MOVE ALL REFERENCES TO ASSET SIZES INTO #DEFINES SO THAT CHANGING THEM IS VERY EASY.
	// Copy this tile of the
	uint32 mapOffset = (my * bg->w) + mx;
	uint32 bgOffset = (y * ZBE_BACKGROUND_TILE_WIDTH) / ZBE_BACKGROUND_BYTES_PER_TILE + x;

	//                                  Each tile is 2 bytes \/
	dmaCopy(bg->data + mapOffset, mapPtr + bgOffset, sizeof(uint16));
}
