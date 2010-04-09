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
		parseZbe(objects);
	}
}

void assets::parseZbe()
{
	// TODO: actually handle the return values on all these file functions.
	//       They're all ignored so there is not fault tolerance.

	// Get the version number out of the zeg file
	uint16 version = load<uint16>(zbeData);
	iprintf("v %d\n", version);

	// Get the total number of assets
	uint32 numAssets = load<uint32>(zbeData);
	iprintf("#assets %d\n", numAssets);

	// Get the number of tiles
	uint32 numTiles = load<uint32>(zbeData);
	iprintf("#gfx %d\n", numTiles);

	// Get all of the gfx tiles
	for (uint32 i = 0; i < numTiles; i++)
	{
		// Make a new assetStatus for the vector
		gfxStatus newAsset;

		// The very first byte of data in gfx tiles is its width, second is height
		uint8 width = load<uint8>(zbeData);
		uint8 height = load<uint8>(zbeData);
		uint8 top = load<uint8>(zbeData);
		uint8 left = load<uint8>(zbeData);
		iprintf(" %d x %d @ (%d, %d)\n", width, height, top, left);
		
		// Set its dimensions
		newAsset.dimensions = vector2D<uint8>(width, height);
		newAsset.topleft = vector2D<uint8>(left, top);
		
		// Set its spriteSize
		newAsset.size = getSpriteSize(width, height);

		// Get the length of this gfx tiles
		newAsset.length = load<uint16>(zbeData);
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
		gfxStatuses.push_back(newAsset);

		// Seek past this object
		fseek(zbeData, newAsset.length, SEEK_CUR);
	}

	// Get the number of palettes
	uint32 numPals = load<uint32>(zbeData);
	iprintf("#pals %d\n", numPals);

	// Get all of the palettes
	for (uint32 i = 0; i < numPals; i++)
	{
		// Make a new status for this palette
		palStatus newAsset;

		// Get the length of this palette
		newAsset.length = load<uint16>(zbeData);
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
		palStatuses.push_back(newAsset);

		// Seek past this object
		fseek(zbeData, newAsset.length, SEEK_CUR);
	}

	// Number of objects
	uint32 numObjects = load<uint32>(zbeData);
	iprintf("#objs %d\n", numObjects);

	// Get all the objects
	for (uint32 i = 0; i < numObjects; i++)
	{
		// Weight of this object
		uint8 weight = load<uint8>(zbeData);
		
		// Number of animations
		uint32 numAnimations = load<uint32>(zbeData);
		iprintf(" %d: %d animations\n", i, numAnimations);

		// Get all the animations
		for (uint32 j = 0; j < numAnimations; j++)
		{
			// Get the number of frames for this animation
			uint16 numFrames = load<uint16>(zbeData);
			iprintf("  %d has %d frames\n", j, numFrames);

			// Make a new animation
			animation anim;

			// Get all the frames
			for (uint32 k = 0; k < numFrames; k++)
			{
				// Make a new frame (Init the gfx pointer to NULL)
				frame thisFrame = {0, 0, NULL, 0};

				// The frame id for this animation frame
				thisFrame.gfxId = load<uint32>(zbeData);
				thisFrame.gfx = gfxStatuses[thisFrame.gfxId];
				
				// The palette id for this animation frame
				thisFrame.palId = load<uint32>(zbeData);
				
				// The time to display this frame
				thisFrame.time = load<uint8>(zbeData);

				// Push on the vector
				anim.frames.push_back(thisFrame);

				iprintf("   %d w/ %d for %d blanks\n", thisFrame.gfxId, thisFrame.palId, thisFrame.time);
			} // this animation
		} // all animations
		
		// make a new objectAsset
		objectAsset newObjectAsset(anim, weight);
		objectAssets.push_back(newObjectAsset);
		
	} // all objects
	
	
	
	/**
	 *
	 *
	 *        NOTE: assets handle and parse the file for the parts that DEFINE
	 *              asset IDs. Anything that ONLY REFERENCES asset IDs should
	 *              be loaded in their respective class (e.g. cine, level, etc.)
	 *
	 *
	 */
	
	// Number of levels
	uint32 numLvls = load<uint32>(zbeData);
	iprintf("#lvls %d\n", numLvls);
	
	// for each level
	for (uint32 i = 0; i < numLvls; i++)
	{
		// Push the current file position on the levelPositions vector
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		levelPositions.push_back(curPos);
		
		/**
		 *   The rest of the level stuff is loaded but forgotten, It should all probably
		 *   replaced with a seek past all this stuff but for now this works, just slowly.
		 */
		
		// number of level objects
		uint32 numLvlObjs = load<uint32>(zbeData);
		
		// for each level objects
		for (uint32 j = 0; j < numLvlObjs; j++)
		{
			// load relevant datas
			uint32 objId = load<uint32>(zbeData);
			uint16 x = load<uint16>(zbeData);
			uint16 y = load<uint16>(zbeData);
		}
	}
}

/*
level* assets::loadLevel(uint32 id)
{
	// Seek to the proper place in the file
	fsetpos(zbeData, &levelPositions);
	
	// Make a new level
	
	
	// number of level objects
	uint32 numLvlObjs = load<uint32>(zbeData);
	iprintf(" #objs %d\n", numLvlObjs);
	
	// for each level objects
	for (uint32 j = 0; j < numLvlObjs; j++)
	{
		// load relevant datas
		uint32 objId = load<uint32>(zbeData);
		uint16 x = load<uint16>(zbeData);
		uint16 y = load<uint16>(zbeData);
		
		// Make a new object
		// object(OamState *Oam, assets *Assets, vector<animation> *anim, int X, int Y, bool Hidden, int MatrixId, int ScaleX, int ScaleY, int Angle, bool Mosaic)
		object *newObj = new object(oam, this, &(objectAssets[objId].animations), x, y, false);
		
		// Add it to the objects vector
		objects->push_back(newObj);
	}
}*/

// Given a width and a height returns an appropriate SpriteSize
SpriteSize assets::getSpriteSize(uint8 width, uint8 height)
{
	// Convert the width and height to 8, 16, 32, or 64 to contain it
	uint8 swidth, sheight;
	if (width <= 8)
		swidth = 8;
	else if (width <= 16)
		swidth = 16;
	else if (width <= 32)
		swidth = 32;
	else if (width <= 64)
		swidth = 64;
	if (height <= 8)
		sheight = 8;
	else if (height <= 16)
		sheight = 16;
	else if (height <= 32)
		sheight = 32;
	else if (height <= 64)
		height = 64;
	
	// Wow this is intense but it's the only way I can come up with to do this.
	switch (swidth)
	{
		case 8:
			switch (sheight)
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
			switch (sheight)
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
			switch (sheight)
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
			switch (sheight)
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

// Loads tiles with id into memory
uint16 *assets::loadGfx(uint32 id)
{
	iprintf("gfx[%d] ", id);

	// See if it's already loaded
	if (gfxStatuses[id].loaded)
	{
		// TODO: this doesn't seem to work.
		iprintf("hit->%x\n", (unsigned int) gfxStatuses[id].offset);

		// Already loaded so return the index
		return gfxStatuses[id].offset;
	}

	iprintf("miss");

	// Need to load it from disk into memory
	// Seek to the proper place in the file
	fsetpos(zbeData, &gfxStatuses[id].position);

	// Request space to load this graphic
	uint16 *mem = oamAllocateGfx(oam, gfxStatuses[id].size, SpriteColorFormat_16Color);
	gfxStatuses[id].offset = mem;

	// Start copying
	uint16 length = gfxStatuses[id].length;
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
			iprintf(" -(%d)-> ", i);
			break;
		}

		// If it made it here, all channels are busy
		if (i == 3)
			i = -1;
	}
	// This is commented out right now because the dma copy up there is asynch.
	//free(data);

	iprintf("%x\n", (unsigned int) gfxStatuses[id].offset);

	return mem;
}

// Loads palette with id into memory
uint8 assets::loadPalette(u32 id)
{
	iprintf("palette[%d] requested\n", id);

	// See if it's already loaded
	if (palStatuses[id].loaded)
	{
		// TODO: this doesn't seem to work.
		iprintf(" cache hit->%d\n", palStatuses[id].index);

		// Already loaded so set the index and return
		return palStatuses[id].index;
	}

	iprintf(" cache miss\n");

	// Need to load it from disk into memory
	// Seek to the proper place in the file
	fsetpos(zbeData, &palStatuses[id].position);

	// Set some variables
	static int curIndex = 0;
	static const int COLORS_PER_PALETTE = 16;

	// Start copying
	uint16 length = palStatuses[id].length;
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
	palStatuses[id].loaded = true;
	palStatuses[id].index = curIndex;
	iprintf(" loaded->%d\n", curIndex);

	// Update the index for the next call
	curIndex++;

	return curIndex - 1;
}


// Reads an integer of type T from the input file
template <class T> T assets::load(FILE *input)
{
	T variable;
	fread(&variable, sizeof(T), 1, input);
	return variable;
}
