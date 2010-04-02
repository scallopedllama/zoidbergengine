#include "assets.h"

assets::assets(char *filename, OamState *table)
{
	// Set variables
	oam = table;

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

		// The very first byte of data in gfx tiles is its width, second is height
		uint8 width = fread8(zbeData);
		uint8 height = fread8(zbeData);
		iprintf(" %d x %d\n", width, height);

		// Set its spriteSize
		newAsset.size = getSpriteSize(width, height);

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
		newAsset.offset = NULL;

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

		// Initialize the offset so the union is set up
		// Then mark that it isn't loaded
		newAsset.index = 0;
		newAsset.loaded = false;

		// Push the new asset on to the array
		palStatus.push_back(newAsset);

		// Seek past this object
		fseek(zbeData, newAsset.length, SEEK_CUR);
	}

	// Number of objects
	uint32 numObjects = fread32(zbeData);
	iprintf("#objs %d\n", numObjects);

	// Get all the objects
	for (uint32 i = 0; i < numObjects; i++)
	{
		// Number of animations
		uint32 numAnimations = fread32(zbeData);
		iprintf(" %d: %d animations\n", i, numAnimations);

		// Get all the animations
		for (uint32 j = 0; j < numAnimations; j++)
		{
			// Get the number of frames for this animation
			uint16 numFrames = fread16(zbeData);
			iprintf("  %d has %d frames\n", j, numFrames);

			// Make a new animation
			animation anim;

			// Get all the frames
			for (uint32 k = 0; k < numFrames; k++)
			{
				// Make a new frame (Init the gfx pointer to NULL)
				frame thisFrame = {0, NULL, 0};

				// The frame id for this animation frame
				thisFrame.gfxId = fread32(zbeData);

				// The time to display this frame
				thisFrame.time = fread8(zbeData);

				// Push on the vector
				anim.frames.push_back(thisFrame);

				iprintf("   %d for %d blanks\n", thisFrame.gfxId, thisFrame.time);
			}
		}
	}
}

// Given a width and a height returns an appropriate SpriteSize
SpriteSize assets::getSpriteSize(uint8 width, uint8 height)
{
	// Wow this is intense but it's the only way I can come up with to do this.
	switch (width)
	{
		case 8:
			switch (height)
			{
				case 8:
					return SpriteSize_8x8;
					break;
				case 16:
					return SpriteSize_8x16;
					break;
				case 32:
					return SpriteSize_8x32;
					break;
				default:
					return SpriteSize_8x32;
					break;
			}
			break;
		case 16:
			switch (height)
			{
				case 8:
					return SpriteSize_16x8;
					break;
				case 16:
					return SpriteSize_16x16;
					break;
				case 32:
					return SpriteSize_16x32;
					break;
			default:
					return SpriteSize_16x32;
					break;
			}
			break;
		case 32:
			switch (height)
			{
				case 8:
					return SpriteSize_32x8;
					break;
				case 16:
					return SpriteSize_32x16;
					break;
				case 32:
					return SpriteSize_32x32;
					break;
				case 64:
					return SpriteSize_32x64;
					break;
				default:
					return SpriteSize_32x64;
			}
			break;
		case 64:
			switch (height)
			{
				case 32:
					return SpriteSize_64x32;
					break;
				case 64:
					return SpriteSize_64x64;
					break;
				default:
					return SpriteSize_64x64;
					break;
			}
			break;
		default:
			return SpriteSize_64x64;
			break;
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
uint16 *assets::loadGfx(uint32 id)
{
	iprintf("gfx[%d] requested\n", id);

	// See if it's already loaded
	if (gfxStatus[id].loaded)
	{
		// TODO: this doesn't seem to work.
		iprintf(" cache hit->%x\n", (unsigned int) gfxStatus[id].offset);

		// Already loaded so return the index
		return gfxStatus[id].offset;
	}

	iprintf(" cache miss\n");

	// Need to load it from disk into memory
	// Seek to the proper place in the file
	fsetpos(zbeData, &gfxStatus[id].position);

	// Request space to load this graphic
	uint16 *mem = oamAllocateGfx(oam, gfxStatus[id].size, SpriteColorFormat_16Color);
	gfxStatus[id].offset = mem;

	// Start copying
	uint16 length = gfxStatus[id].length;
	uint16 *data = (uint16*) malloc(length * sizeof(uint8));
	if (fread(data, sizeof(uint8), length, zbeData) < length)
		iprintf(" data load error\n");

	// Find the next free DMA channel and use it to copy the gfx into video memory
	// It'll try channel 3 first because everybody seems to use that for this.
	for (int i = 3; i <= 3; i++)
	{
		if (!dmaBusy(i))
		{
			DC_FlushRange(data, length);
			dmaCopyHalfWordsAsynch(i, data, mem, length);
			iprintf(" Copied using DMA bus %d\n", i);
			break;
		}
		else
			iprintf(" dma bus %d busy\n", i);

		// If it made it here, all channels are busy
		if (i == 3)
			i = -1;
	}
	// This is commented out right now because the dma copy up there is asynch.
	//free(data);

	iprintf(" loaded->%x\n", (unsigned int) gfxStatus[id].offset);

	return mem;
}

// Loads palette with id into memory
uint8 assets::loadPalette(u32 id)
{
	iprintf("palette[%d] requested\n", id);

	// See if it's already loaded
	if (palStatus[id].loaded)
	{
		// TODO: this doesn't seem to work.
		iprintf(" cache hit->%d\n", palStatus[id].index);

		// Already loaded so set the index and return
		return palStatus[id].index;
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

	// Find the next free DMA channel and use it to copy the palette into video memory
	// It'll try channel 3 first because everybody seems to use that for this.
	for (int i = 3; i <= 3; i++)
	{
		if (!dmaBusy(i))
		{
			DC_FlushRange(data, length);
			dmaCopyHalfWordsAsynch(i, data, &SPRITE_PALETTE[curIndex * COLORS_PER_PALETTE], length);
			iprintf(" Copied using DMA bus %d\n", i);
			break;
		}
		else
			iprintf(" dma bus %d busy\n", i);

		// If it made it here, all channels are busy
		if (i == 3)
			i = -1;
	}
	// This is commented out right now because the dma copy up there is asynch.
	//free(data);

	// Update some variables
	palStatus[id].loaded = true;
	palStatus[id].index = curIndex;
	iprintf(" loaded->%d\n", curIndex);

	// Update the index for the next call
	curIndex++;

	return curIndex - 1;
}
