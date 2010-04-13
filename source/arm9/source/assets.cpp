#include "assets.h"

assets::assets(char *input, OamState *table)
{
	// Set variables
	oam = table;
	zbeFile = input;

	// Parse the file
	parseZbe();
}

void assets::parseZbe()
{
	// TODO: actually handle the return values on all these file functions.
	//       They're all ignored so there is not fault tolerance.

	openFile();

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
		// Make a new gfxAsset for the vector
		gfxAsset *newAsset = new gfxAsset;

		// The very first byte of data in gfx tiles is its width, second is height
		uint8 width = load<uint8>(zbeData);
		uint8 height = load<uint8>(zbeData);
		uint8 top = load<uint8>(zbeData);
		uint8 left = load<uint8>(zbeData);
		iprintf(" %d x %d @ (%d, %d)\n", width, height, top, left);

		// Set its dimensions
		newAsset->dimensions = vector2D<uint8>(width, height);
		newAsset->topleft = vector2D<uint8>(left, top);

		// Set its spriteSize
		newAsset->size = getSpriteSize(width, height);

		// Get the length of this gfx tiles
		newAsset->length = load<uint16>(zbeData);
		iprintf(" len %d\n, ", newAsset->length);

		// Add the current position in the file to the vector
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		newAsset->position = curPos;

		// Set that it is not loaded, assign a value to the union
		// to get it using the proper value
		newAsset->loaded = false;
		newAsset->offset = NULL;

		// Push this asset on the array
		gfxAssets.push_back(newAsset);

		// Seek past this object
		fseek(zbeData, newAsset->length, SEEK_CUR);
	}

	// Get the number of palettes
	uint32 numPals = load<uint32>(zbeData);
	iprintf("#pals %d\n", numPals);

	// Get all of the palettes
	for (uint32 i = 0; i < numPals; i++)
	{
		// Make a new paletteAsset
		paletteAsset *newAsset = new paletteAsset;

		// Get the length of this palette
		newAsset->length = load<uint16>(zbeData);
		iprintf(" %d's len %d\n", i, newAsset->length);

		// Add the current position in the file to the vector
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		newAsset->position = curPos;

		// Initialize the offset so the union is set up
		// Then mark that it isn't loaded
		newAsset->index = 0;
		newAsset->loaded = false;

		// Push the new asset on to the array
		paletteAssets.push_back(newAsset);

		// Seek past this object
		fseek(zbeData, newAsset->length, SEEK_CUR);
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

		// Make the objectAsset and allocate for enough animations
		objectAsset *newAsset = new objectAsset(weight);
		newAsset->animations = new frameAsset**[numAnimations + 1];
		// null terminate that dimension
		newAsset->animations[numAnimations] = NULL;

		// Get all the animations
		for (uint32 j = 0; j < numAnimations; j++)
		{
			// Get the number of frames for this animation
			uint16 numFrames = load<uint16>(zbeData);
			iprintf("  %d has %d frames\n", j, numFrames);

			// Allocate enough pointers for this animation
			newAsset->animations[j] = new frameAsset*[numFrames + 1];
			// null terminate it
			newAsset->animations[j][numFrames] = NULL;

			// Get all the frames
			for (uint32 k = 0; k < numFrames; k++)
			{
				// Make a new frameAsset (Init the gfx pointer to NULL)
				frameAsset *thisFrame = new frameAsset;

				// The gfx for this animation frame
				uint32 gfxId = load<uint32>(zbeData);
				thisFrame->gfx = gfxAssets[gfxId];

				// The palette for this animation frame
				uint32 palId = load<uint32>(zbeData);
				thisFrame->pal = paletteAssets[palId];

				// The time to display this frame
				thisFrame->time = load<uint8>(zbeData);

				// Set it up on the array
				newAsset->animations[j][k] = thisFrame;

				iprintf("   %d w/ %d for %d blanks\n", gfxId, palId, thisFrame->time);
			} // this animation

		} // all animations

		// make a new objectAsset
		objectAssets.push_back(newAsset);

	} // all objects


	// Number of levels
	uint32 numLvls = load<uint32>(zbeData);
	iprintf("#lvls %d\n", numLvls);

	// for each level
	for (uint32 i = 0; i < numLvls; i++)
	{
		// Make a new levelAsset
		levelAsset *newAsset = new levelAsset;
		newAsset->objects = NULL;

		// Set the location variable
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		newAsset->position = curPos;

		// Push this asset onto the levelAssets vector
		levelAssets.push_back(newAsset);

		// number of level objects
		uint32 numLvlObjs = load<uint32>(zbeData);

		// for each level objects
		// TODO: replace this with a bit of seeking magic to get rid of warnings and speed things up a bit
		for (uint32 j = 0; j < numLvlObjs; j++)
		{
			// load relevant datas
			uint32 objId = load<uint32>(zbeData);
			uint16 x = load<uint16>(zbeData);
			uint16 y = load<uint16>(zbeData);
		}
	}

	closeFile();
}


// TODO: properly delete all the stuff dynamically allocated in the parse function!


// Load and return a level's metadata
levelAsset *assets::loadLevel(uint32 id)
{
	// open the file
	openFile();

	// keep track of the last one opened
	static levelAsset *lvl = NULL;

	// Clear out the last one if this isn't the first time
	if (lvl)
	{
		lvl->clear();
		lvl = NULL;
	}

	// Update last
	lvl = levelAssets[id];

	// Seek to the proper place in the file
	fsetpos(zbeData, &(lvl->position));
	iprintf("lvl %d requested\n", id);

	// number of level objects
	uint32 numLvlObjs = load<uint32>(zbeData);
	iprintf(" #objs %d\n", numLvlObjs);

	// Allocate enough space for all the level objects
	lvl->objects = new levelObjectAsset*[numLvlObjs + 1];
	lvl->objects[numLvlObjs] = NULL;

	// for each level object
	for (uint32 j = 0; j < numLvlObjs; j++)
	{
		// load relevant datas
		uint32 objId = load<uint32>(zbeData);
		uint16 x = load<uint16>(zbeData);
		uint16 y = load<uint16>(zbeData);
		iprintf("  #%d: obj%d at (%d, %d)\n", (int) j, (int) objId, (int) x, (int) y);

		// Make a new levelObjectAsset and add it to the vector
		levelObjectAsset *lvlObj = new levelObjectAsset(vector2D<float>(float(x), float(y)), objectAssets[objId]);
		lvl->objects[j] = lvlObj;
	}

	//close the file
	closeFile();

	// Return that levelAsset
	return lvl;
}


// Loads tiles with id into memory
uint16 *assets::loadGfx(gfxAsset *gfx)
{
	if (!gfx)
		return 0;

	iprintf("gfx load, ");

	// See if it's already loaded
	if (gfx->loaded)
	{
		// TODO: this doesn't seem to work.
		iprintf("hit->%x\n", (unsigned int) gfx->offset);

		// Already loaded so return the index
		return gfx->offset;
	}

	// Need to load the gfx from the file
	openFile();

	if(ferror(zbeData))
	{
		iprintf("\n data error: %s\n", strerror(errno));
		while(true);
	}

	iprintf("miss");

	// Need to load it from disk into memory
	// Seek to the proper place in the file
	if (fsetpos(zbeData, &(gfx->position)))
	{
		iprintf("\n seek error: %s\n", strerror(errno));
		while(true);
	}

	// Request space to load this graphic
	uint16 *mem = oamAllocateGfx(oam, gfx->size, SpriteColorFormat_16Color);
	gfx->offset = mem;

	// Start copying
	uint16 length = gfx->length;
	uint16 *data = (uint16*) malloc(length * sizeof(uint8));
	if (fread(data, sizeof(uint8), length, zbeData) < length)
		iprintf(" data load error\n");

	DC_FlushRange(data, length);
	dmaCopyHalfWords(3, data, mem, length);

	free(data);

	iprintf("%x\n", (unsigned int) gfx->offset);

	// Close the File before returning
	closeFile();

	return mem;
}


// Loads palette with id into memory
uint8 assets::loadPalette(paletteAsset *pal)
{
	iprintf("palette load, ");

	// See if it's already loaded
	if (pal->loaded)
	{
		// TODO: this doesn't seem to work.
		iprintf(" cache hit->%d\n", pal->index);

		// Already loaded so set the index and return
		return pal->index;
	}

	// Need to load the gfx from the file
	openFile();

	iprintf(" cache miss\n");

	// Need to load it from disk into memory
	// Seek to the proper place in the file
	fsetpos(zbeData, &(pal->position));

	// Set some variables
	static int curIndex = 0;
	static const int COLORS_PER_PALETTE = 16;

	// Start copying
	uint16 length = pal->length;
	void *data = malloc(length * sizeof(u8));
	if (fread(data, sizeof(u8), length, zbeData) < length)
		iprintf(" data load error\n");

	DC_FlushRange(data, length);
	dmaCopyHalfWordsAsynch(3, data, &SPRITE_PALETTE[curIndex * COLORS_PER_PALETTE], length);
	free(data);

	// Update some variables
	pal->loaded = true;
	pal->index = curIndex;
	iprintf(" loaded->%d\n", curIndex);

	// Update the index for the next call
	curIndex++;

	// Close the File before returning
	closeFile();

	return curIndex - 1;
}


// Reads an integer of type T from the input file
template <class T> T assets::load(FILE *input)
{
	T variable;
	if(fread(&variable, sizeof(T), 1, input) < 1)
	{
		iprintf("fread error\n");
		while (true);
	}
	if(ferror(input))
	{
		iprintf("file error: %s\n", strerror(errno));
		while (true);
	}
	return variable;
}


// Given a width and a height returns an appropriate SpriteSize
SpriteSize assets::getSpriteSize(uint8 width, uint8 height)
{
	// Convert the width and height to 8, 16, 32, or 64 to contain it
	uint8 swidth = 64, sheight = 64;
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
