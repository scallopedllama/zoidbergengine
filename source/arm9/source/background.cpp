#include "background.h"

// loads up a background
int background::loadBackground(levelBackgroundAsset *metadata, gfxAsset *tileset)
{
	distance = metadata->distance;
	layer = metadata->layer;

	// Load up the backgroundAsset to get the map data
	zbeAssets->loadBackground(metadata);
	bg = metadata->background;

	// Make sure tiles data is loaded
	loadGfx(tileset);

	// Init the background
	// mapBases are 2KB, tileBases are 16KB, and they overlap.
	// Map tiles occupy background->tileset->length B / 1024 (B / KB) / 2 (KB / offset) mapBases
	//   that needs to be rounded up, though so see if it fits perfectly, if it doesn't add one
	int tileSize = (tileset->length % 2048 == 0) ? tileset->length / 2048 : tileset->length / 2048 + 1;

	// Map data will ALWAYS occupy (512 px / 8 (px / tile)) * (256 px / 8 (px/tile)) * 2 (B / tile) / 1024 (B / KB) = 4 KB = 2 mapBases
	int mapBase = tileSize + (layer * 2);

	// bgId = bgInit(layer, bgType, bgSize, mapBase, tileBase)
	background->bgId = bgInit(layer, BgType_Text4bpp,  ZOIDBERG_BACKGROUND_SIZE, mapBase, 0);
	// Need to reverse the layer value to get the proper priority
	bgSetPriority(background->bgId, 3 - layer);
	iprintf(" Init'd, id %d, mb %d, ts %d\n", backgroundId, mapBase, tileSize);

	// Copy tiles into video memory (If not already loaded)
	if (!tileset->vmLoaded)
	{
		iprintf("  ld %dB, cp tileset (%x)\n", tileset->length, (unsigned int) bgGetGfxPtr(background->bgId));
		DC_FlushRange(tileset->data, tileset->length);
		dmaCopy(tileset->data, bgGetGfxPtr(background->bgId), tileset->length);
		tileset->vmLoaded = true;
		// unload the tileset (doesn't need to stay in main memory
		freeGfx(tileset);
	}
	else
		iprintf("  tileset already loaded\n");

	// Copy the visible part of the background into video memory
	DC_FlushRange(bg->data, bg->length);
	uint16 *mapPtr = bgGetMapPtr(bg->bgId);
	iprintf("  load map -> %x\n", (int) mapPtr);

	// Row Major Order
	// We need to copy enough tiles to fill the height of the screen plus some buffer
	for (uint32 y = 0; y < bg->h; y++)
	{
		for (uint32 x = 0; x < bg->w; x++)
		{
			// TODO: MOVE ALL REFERENCES TO ASSET SIZES INTO #DEFINES SO THAT CHANGING THEM IS VERY EASY.
			// Copy this tile of the map                                                      Each tile is 2 bytes \/
			uint32 mapOffset = (y * bg->w) + x;
			uint32 bgOffset = (y * ZOIDBERG_BACKGROUND_TILE_WIDTH) / ZOIDBERG_BACKGROUND_BYTES_PER_TILE + x;
			dmaCopy(bg->data + mapOffset, mapPtr + bgOffset, sizeof(uint16));
		}
	}
	iprintf(" copied map\n");

	// Load, then copy all the palettes into memory
	for (uint8 i = 0; i < numPalettes; i++)
	{
		iprintf(" load pal %d\n ", paletteIds[i]);

		// Load up the palette
		paletteAsset *pal = paletteAssets[paletteIds[i]];
		loadPalette(pal);

		// Copy the palette into memory, remember to offset it from the last one
		iprintf(" and copy %dB (%x)\n", pal->length, (unsigned int) BG_PALETTE + offset);
		DC_FlushRange(pal->data, pal->length);
		dmaCopy(pal->data, BG_PALETTE + offset * 16, pal->length);
		offset ++;
	}

	return background->bgId;
}
