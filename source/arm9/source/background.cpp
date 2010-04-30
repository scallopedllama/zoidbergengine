#include "background.h"

// loads up a background
background::background(levelBackgroundAsset *metadata, gfxAsset *tileset, uint8 paletteOffset)
{
	lastScreenOffset = screenOffset;
	distance = metadata->distance;
	layer = metadata->layer;
	lastScroll = vector2D<float>(128.0, 32.0);
	leftOverScroll = vector2D<float>(0.0, 0.0);

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
/*
	// Row Major Order
	for (uint8 y = 0; y < ZBE_BACKGROUND_TILE_HEIGHT; y++)
	{
		for (uint8 x = 0; x < ZBE_BACKGROUND_TILE_WIDTH; x++)
		{
			// Copy the tile
			copyTile(x, y, int(screenOffset.x - 128) / 8 + x, int(screenOffset.y - 32) / 8  + y);
		}
	}*/

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
	bgSetScroll(backgroundId, (int) thisScroll.x, (int) thisScroll.y);

	// Add the amount of background left over from past updates that is replacable
	vector2D<float> toReplace = vector2D<float>(displacement.x + leftOverScroll.x, displacement.y + leftOverScroll.y);

	//The number of columns to replace in those dimensions
	int repRows = int(toReplace.y / 8);
	int repCols = int(toReplace.x / 8);

	// Update leftOverScroll
	leftOverScroll.x = toReplace.x - repCols * 8;
	leftOverScroll.y = toReplace.y - repRows * 8;

	// Return if not replacing anything
	// TODO: turn this back on
	//if (repRows == 0 && repCols == 0)
	//	return;

	// where to begin the replacement in the vm background map
	vector2D<float> vmBgMapRep((thisScroll.x - 128) / 8, (thisScroll.y - 32) / 8);

	// where to copy the replacement tiles from in mm background map
	vector2D<float> mmBgMapRepTL((screenOffset.x - 128) / 8, (screenOffset.y - 32) / 8);
	vector2D<float> mmBgMapRepBR((screenOffset.x + SCREEN_WIDTH + 128) / 8, (screenOffset.y + SCREEN_HEIGHT + 32) / 8);

/*
	// This fixes some of the missed rows / columns issues
	if (repRows != 0)
	{
		if (repRows < 0)
		{
			repRows-=5;
			vmBgMapRep.y+=1.0;
			mmBgMapRepBR.y += 1.0;
			mmBgMapRepTL.y += 1.0;
		}
		else
		{
			repRows+=5;
			vmBgMapRep.y-=1.0;
			mmBgMapRepBR.y -= 1.0;
			mmBgMapRepTL.y -= 1.0;
		}
	}
	if (repCols != 0)
	{
		if (repCols < 0)
		{
			repCols-=5;
			vmBgMapRep.x += 1.0;
			mmBgMapRepBR.x += 1.0;
			mmBgMapRepTL.x += 1.0;
		}
		else{
			repCols+=5;
			vmBgMapRep.x -= 1.0;
			mmBgMapRepBR.x -= 1.0;
			mmBgMapRepTL.x -= 1.0;
		}
	}*/

	//       --------------------------------
	consoleClear();
	printf("sO: (%f, %f)\n", screenOffset.x, screenOffset.y);
	printf("los: %f, %f\n", leftOverScroll.x, leftOverScroll.y);
	printf("bg's lvl map coords:\n");
	printf("  (%f, %f) to \n  (%f, %f)\n", mmBgMapRepTL.x, mmBgMapRepTL.y, mmBgMapRepBR.x, mmBgMapRepBR.y);
	printf("rep %d, %d at\n  (%f, %f)\n", repRows, repCols, vmBgMapRep.x, vmBgMapRep.y);
	printf("bg %d x %d\n", bg->w, bg->h);

	lastScreenOffset = screenOffset;
	lastScroll = thisScroll;//pause();

	if (repRows == 0 && repCols == 0)
		return;

	// Now we just need to copy the tiles
	// replace repRows rows
	for (int r = 0; r != repRows;)
	{
		int repStart = 0; // (repRows > 0) ? 0 : 16;
		int repEnd = ZBE_BACKGROUND_TILE_WIDTH - repCols; //(repRows > 0) ? ZBE_BACKGROUND_TILE_WIDTH - 16 : ZBE_BACKGROUND_TILE_WIDTH;

		// Replace Visible area + half of buffer
		for (int c = repStart; c <= repEnd; c++)
		{
			// Copy it up
			int mmMapTileY = (displacement.y > 0) ? mmBgMapRepBR.y + r : mmBgMapRepTL.y + r;
			copyTile(vmBgMapRep.x + c, vmBgMapRep.y + r, mmBgMapRepTL.x + c, mmMapTileY);
		}

		iprintf("reprow'd\n");

		// Increment or decrement r
		if (repRows > 0) r++;
		else r--;
	}

	// replace repCols columns
	for (int c = 0; c != repCols;)
	{
		int repStart = 0;//(repCols > 0) ? 0 : 4;
		int repEnd = ZBE_BACKGROUND_TILE_HEIGHT - repRows; //(repCols > 0) ? ZBE_BACKGROUND_TILE_HEIGHT - 4 : ZBE_BACKGROUND_TILE_HEIGHT;

		// Replace visible area + half of buffer
		for (int r = repStart; r <= repEnd; r++)
		{
			// Copy it up
			int mmMapTileX = (displacement.x > 0) ? mmBgMapRepBR.x + c : mmBgMapRepTL.x + c;
			copyTile(vmBgMapRep.x + c, vmBgMapRep.y + r, mmMapTileX, mmBgMapRepTL.y + r);
		}

		iprintf("repcol'd\n");

		// Increment or decrement c
		if (repCols > 0) c++;
		else c--;
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

// TODO: remove this
	for(int i = 0; i<50000; i++);
}
