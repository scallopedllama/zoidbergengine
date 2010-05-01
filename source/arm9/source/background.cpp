#include "background.h"

// loads up a background
background::background(levelBackgroundAsset *metadata, gfxAsset *tileset, uint8 paletteOffset)
{
	distance = metadata->distance;
	layer = metadata->layer;
	lastScreenOffset = screenOffset;
	lastBgMapRepTL = vector2D<int>(int(screenOffset.x - 128) / 8 - 1, int(screenOffset.y - 32) / 8 - 1);
	lastBgMapRepBR = vector2D<int>(int(screenOffset.x + SCREEN_WIDTH + 128) / 8, int(screenOffset.y + SCREEN_HEIGHT + 32) / 8);


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

	// Flush the cache in that region of memory to prevent screwups
	DC_FlushRange(bg->data, bg->length);
	mapPtr = bgGetMapPtr(backgroundId);
	iprintf("  load map -> %x\n", (int) mapPtr);

	// Redraw the screen's visible background
	redraw();

	iprintf(" copied map\n");
}


// Replaces the entire visible background
void background::redraw()
{
	// Row Major Order
	for (uint8 y = 0; y < ZBE_BACKGROUND_TILE_HEIGHT; y++)
	{
		for (uint8 x = 0; x < ZBE_BACKGROUND_TILE_WIDTH; x++)
		{
			// Copy the tile
			//copyTile(x, y, int(screenOffset.x) / 8 + x, int(screenOffset.y) / 8  + y);
		}
	}
	consoleClear();
	iprintf("redraw");
	pause();
}


// Updates the scroll position of this background
void background::update()
{
	// Find out how much things have moved
	vector2D<float> displacement = vector2D<float>(screenOffset.x - lastScreenOffset.x , screenOffset.y - lastScreenOffset.y);

	// scroll the background (mod by bg dimensions because hardware will crash if the value gets too big)
	bgSetScroll(backgroundId, int(screenOffset.x) % (ZBE_BACKGROUND_TILE_WIDTH * 8), int(screenOffset.y) % (ZBE_BACKGROUND_TILE_HEIGHT * 8));

	// where to copy the replacement tiles from in background map
	vector2D<int> bgMapRepTL(int(screenOffset.x - 128) / 8 - 1, int(screenOffset.y - 32) / 8 - 1);
	vector2D<int> bgMapRepBR(int(screenOffset.x + SCREEN_WIDTH + 128) / 8, int(screenOffset.y + SCREEN_HEIGHT + 32) / 8);
	/*copyTile(int(screenOffset.x) / 8, int(screenOffset.y) / 8, 1, 1);
	copyTile(int(screenOffset.x + SCREEN_WIDTH) / 8 - 1, int(screenOffset.y + SCREEN_HEIGHT) / 8 - 1, 2, 2);*/

	consoleClear();
	printf("d: (%f, %f)\n", displacement.x, displacement.y);
	printf("SO: (%f, %f)\n", screenOffset.x, screenOffset.y);
	printf("lSO: (%f, %f\n", lastScreenOffset.x, lastScreenOffset.y);
	printf("TL:  (%d, %d),  BR: (%d, %d)", bgMapRepTL.x, bgMapRepTL.y, bgMapRepBR.x, bgMapRepBR.y);
	printf("lTL: (%d, %d), lBR: (%d, %d)", lastBgMapRepTL.x, lastBgMapRepTL.y, lastBgMapRepBR.x, lastBgMapRepBR.y);

	// If there are more tiles to replace than there are in the screen, just replace the whole thing and be done with it
	if (((int) displacement.x % SCREEN_WIDTH) * ((int) displacement.y % SCREEN_HEIGHT) > SCREEN_HEIGHT * SCREEN_WIDTH)
	{
		// Redraw the whole screen
		redraw();
		return;
	}

	// Scrolling LEFT
	if (displacement.x < -8)
	{
		// Replace Columns
		for (int c = lastBgMapRepTL.x + 1; c >= bgMapRepTL.x; c--)
		{
			// Replace Rows
			for (int r = 3; r < ZBE_BACKGROUND_TILE_HEIGHT - 1; r++)
			{
				// Copy it up
				copyTile(c, lastBgMapRepTL.y + r, c, lastBgMapRepTL.y + r);
			}
		}
	}

	// Scrolling RIGHT
	if (displacement.x > 8)
	{
		// Replace Columns
		for (int c = lastBgMapRepBR.x - 1; c <= bgMapRepBR.x; c++)
		{
			// Replace Rows
			for (int r = 3; r < ZBE_BACKGROUND_TILE_HEIGHT - 1; r++)
			{
				// Copy it up
				copyTile(c, lastBgMapRepTL.y + r, c, lastBgMapRepTL.y + r);
			}
		}
	}

	// Scrolling UP
	if (displacement.y < -8)
	{
		// Replace rows
		for (int r = lastBgMapRepTL.y + 1; r >= bgMapRepTL.y; r--)
		{
			// Each column in the row
			for (int c = 9; c < ZBE_BACKGROUND_TILE_WIDTH - 7; c++)
			{
				// Copy it up
				copyTile(lastBgMapRepTL.x + c, r, lastBgMapRepTL.x + c, r);
			}
		}
	}

	// Scrolling DOWN
	if (displacement.y > 8)
	{
		// Replace rows
		for (int r = lastBgMapRepBR.y - 1; r <= bgMapRepBR.y; r++)
		{
			// Each column in the row
			for (int c = 9; c < ZBE_BACKGROUND_TILE_WIDTH - 7; c++)
			{
				// Copy it up
				copyTile(lastBgMapRepTL.x + c, r, lastBgMapRepTL.x + c, r);
			}
		}
	}






	// Save some values to avoid havning to re-calculate them.
	// Only change the value if a row/col was replaced in that direction
	if (displacement.x > 8 || displacement.x < -8)
	{
		lastScreenOffset.x = screenOffset.x;
		lastBgMapRepBR.x = bgMapRepBR.x;
		lastBgMapRepTL.x = bgMapRepTL.x;
	}
	if (displacement.y > 8 || displacement.y < -8)
	{
		lastScreenOffset.y = screenOffset.y;
		lastBgMapRepBR.y = bgMapRepBR.y;
		lastBgMapRepTL.y = bgMapRepTL.y;
	}
}


void background::copyTile(int x, int y, int mx, int my)
{
	// Make sure all these values are within bounds
	while (x < 0) x += ZBE_BACKGROUND_TILE_WIDTH;
	if (x >= ZBE_BACKGROUND_TILE_WIDTH) x = x % ZBE_BACKGROUND_TILE_WIDTH;
	while (y < 0) y += ZBE_BACKGROUND_TILE_HEIGHT;
	if (y >= ZBE_BACKGROUND_TILE_HEIGHT) y = y % ZBE_BACKGROUND_TILE_HEIGHT;

	while (mx < 0) mx += bg->w;
	if (mx >= int(bg->w)) mx = mx % bg->w;
	while (my < 0) my += bg->h;
	if (my >= int(bg->h)) my = my % bg->h;

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
