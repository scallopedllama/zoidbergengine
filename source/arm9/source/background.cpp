#include "background.h"

// loads up a background
background::background(levelBackgroundAsset *metadata, gfxAsset *tileset, uint8 paletteOffset)
{
	lastScreenOffset = screenOffset;
	distance = metadata->distance;
	layer = metadata->layer;
	lastScroll = vector2D<float>(128.0, 32.0);

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

	// Copy the visible part of the background into video memory
	DC_FlushRange(bg->data, bg->length);
	mapPtr = bgGetMapPtr(backgroundId);
	iprintf("  load map -> %x\n", (int) mapPtr);

	// Row Major Order
	for (uint8 y = 0; y < ZBE_BACKGROUND_TILE_HEIGHT; y++)
	{
		for (uint8 x = 0; x < ZBE_BACKGROUND_TILE_WIDTH; x++)
		{
			// Copy the tile
			copyTile(x, y, int(screenOffset.x - 128) / 8 + x, int(screenOffset.y - 32) / 8  + y);
		}
	}
	iprintf(" copied map\n");
}


// Updates the scroll position of this background
void background::update()
{
	// Find out how much things have moved
	vector2D<float> displacement = vector2D<float>(screenOffset.x - lastScreenOffset.x , screenOffset.y - lastScreenOffset.y);
	// Figure out where the background will be scrolled for this frame
	vector2D<float> thisScroll = vector2D<float>(lastScroll.x + displacement.x, lastScroll.y + displacement.y);

	// scroll the background
	//bgScroll(backgroundId, (int) thisScroll.x / 8, (int) thisScroll.y / 8);
	bgScroll(backgroundId, (int) displacement.x, (int) displacement.y);

	//The number of columns to replace in those dimensions
	int repRows = abs(int(displacement.y)) / 8;
	int repCols = abs(int(displacement.x)) / 8;

	// Return if not replacing anything
	if (repRows == 0 && repCols == 0)
		return;

	// where to begin the replacement in the vm background map
	vector2D<int> vmBgMapRep((thisScroll.x - 128) / 8, (thisScroll.y - 32) / 8);
	// Make sure they're not negative
	if (vmBgMapRep.x < 0) vmBgMapRep.x += (ZBE_BACKGROUND_TILE_WIDTH * 8);
	if (vmBgMapRep.y < 0) vmBgMapRep.y += (ZBE_BACKGROUND_TILE_HEIGHT * 8);

	// where to copy the replacement tiles from in mm background map
	vector2D<int> mmBgMaprep;
	// Scrolling left
	if (displacement.x < 0) mmBgMaprep.x = (screenOffset.x - 128) / 8;
	// Scrolling right
	else                    mmBgMaprep.x = (screenOffset.x + SCREEN_WIDTH + 128) / 8;
	// Scrolling Up
	if (displacement.y < 0) mmBgMaprep.y = (screenOffset.y - 32) / 8;
	// Scrolling right
	else                    mmBgMaprep.y = (screenOffset.y + SCREEN_HEIGHT + 32) / 8;
	//       --------------------------------
	iprintf("lso (%d, %d), so (%d, %d)\n", (int) lastScreenOffset.x, (int) lastScreenOffset.y, (int) screenOffset.x, (int) screenOffset.y);
	iprintf("d (%d, %d)               \n", (int) displacement.x, (int) displacement.y);
	iprintf("ls (%d, %d), ts (%d, %d) \n", (int) lastScroll.x, (int) lastScroll.y, (int) thisScroll.x, (int) thisScroll.y);
	iprintf("Scrolled to %d, %d       \n", (int) displacement.x, (int) displacement.y);
	iprintf("Replacing %d cols at %d  \n", repCols, vmBgMapRep.x);
	iprintf("Replacing %d rows at %d  \n\n", repRows, vmBgMapRep.y);//pause();
/*
	// Now we just need to copy the tiles
	// replace repRows rows
	for (int r = 0; r < repRows; r++)
	{
		// Every column tile for this row
		for (int c = 0; c < ZBE_BACKGROUND_TILE_WIDTH; c++)
		{
			// Copy it up
			int screenMapTileX = (vmBgMapRep.x + c) % ZBE_BACKGROUND_TILE_WIDTH;
			int screenMapTileY = (vmBgMapRep.y + r) % ZBE_BACKGROUND_TILE_HEIGHT;
			int memMapTileX = (mmBgMaprep.x + c) % ZBE_BACKGROUND_TILE_WIDTH;
			int memMapTileY = (mmBgMaprep.y + r) % ZBE_BACKGROUND_TILE_HEIGHT;
			copyTile(screenMapTileX, screenMapTileY, memMapTileX, memMapTileY);
		}
	}

	// replace repCols columns
	for (int c = 0; c < repCols; c++)
	{
		// Every row tile for this column
		for (int r = 0; r < ZBE_BACKGROUND_TILE_HEIGHT; r++)
		{
			// Copy it up
			int screenMapTileX = (vmBgMapRep.x + c) % ZBE_BACKGROUND_TILE_WIDTH;
			int screenMapTileY = (vmBgMapRep.y + r) % ZBE_BACKGROUND_TILE_HEIGHT;
			int memMapTileX = (mmBgMaprep.x + c) % ZBE_BACKGROUND_TILE_WIDTH;
			int memMapTileY = (mmBgMaprep.y + r) % ZBE_BACKGROUND_TILE_HEIGHT;
			copyTile(screenMapTileX, screenMapTileY, memMapTileX, memMapTileY);
		}
	}*/

	lastScreenOffset = screenOffset;
	lastScroll = thisScroll;
}


void background::copyTile(int x, int y, int mx, int my)
{
	// Make sure all these values are within bounds
	if (x < 0) x += ZBE_BACKGROUND_TILE_WIDTH;
	if (x >= ZBE_BACKGROUND_TILE_WIDTH) x = x % ZBE_BACKGROUND_TILE_WIDTH;
	if (y < 0) y += ZBE_BACKGROUND_TILE_HEIGHT;
	if (y > ZBE_BACKGROUND_TILE_HEIGHT) y = y % ZBE_BACKGROUND_TILE_HEIGHT;

	if (mx < 0) mx += bg->w;
	if (mx >= bg->w) mx = mx % bg->w;
	if (my < 0) my += bg->h;
	if (my >= bg->h) my = my % bg->h;

	// The tiles on the right half of the background actually go AFTER the left half.
	if (x >= SCREEN_WIDTH / 8)
	{
		y += ZBE_BACKGROUND_TILE_HEIGHT;
		x -= SCREEN_WIDTH / 8;
	}

	// TODO: MOVE ALL REFERENCES TO ASSET SIZES INTO #DEFINES SO THAT CHANGING THEM IS VERY EASY.
	// Copy this tile of the
	uint32 mapOffset = (my * bg->w) + mx;
	uint32 bgOffset = (y * ZBE_BACKGROUND_TILE_WIDTH) / ZBE_BACKGROUND_BYTES_PER_TILE  + x;

	//                                  Each tile is 2 bytes \/
	dmaCopy(bg->data + mapOffset, mapPtr + bgOffset, sizeof(uint16));
}
