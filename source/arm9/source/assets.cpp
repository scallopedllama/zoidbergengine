#include "assets.h"

assets::assets(char* input, OamState *table)
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

	// Open up the file to parse
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
		newAsset->mmLoaded = newAsset->vmLoaded = false;
		newAsset->offset = NULL;

		// Push this asset on the array
		gfxAssets.push_back(newAsset);

		// Seek past this object
		fseek(zbeData, newAsset->length, SEEK_CUR);
	}

	// Get the number of background tilesets
	uint32 numTilesets = load<uint32>(zbeData);
	iprintf("#tilesets %d\n", numTilesets);

	// Load up all those tilesets
	for (uint32 i = 0; i < numTilesets; i++)
	{
		// Make a new gfxAsset for the vector
		gfxAsset *newAsset = new gfxAsset;

		// Load up the length
		newAsset->length = load<uint16>(zbeData);
		iprintf(" #%d: %dB\n", i, newAsset->length);

		// Save the position
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		newAsset->position = curPos;

		// Seek past the data
		fseek(zbeData, newAsset->length, SEEK_CUR);

		// Duh.. add it to the tilesetAssets vector
		tilesetAssets.push_back(newAsset);
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

		// Add the current position in the file to the asset
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		newAsset->position = curPos;

		// Initialize the offset so the union is set up
		// Then mark that it isn't loaded
		newAsset->index = 0;
		newAsset->mmLoaded = newAsset->vmLoaded = false;

		// Push the new asset on to the array
		paletteAssets.push_back(newAsset);

		// Seek past this object
		fseek(zbeData, newAsset->length, SEEK_CUR);
	}


	// Number of backrounds
	uint32 numBackgrounds = load<uint32>(zbeData);
	iprintf("#bgs %d\n", numBackgrounds);

	// Get all the backgrounds
	for (unsigned int i = 0; i < numBackgrounds; i++)
	{
		// make a new backgroundAsset
		backgroundAsset *newAsset = new backgroundAsset();

		// load up the reference to its tileset
		uint32 tilesetId = load<uint32>(zbeData);
		newAsset->tileset = tilesetAssets[tilesetId];

		// set its width and height
		newAsset->w = load<uint32>(zbeData);
		newAsset->h = load<uint32>(zbeData);

		// Save the position of all the tiles data
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		newAsset->position = curPos;

		// Get number of palettes
		uint8 numPalettes = load<uint8>(zbeData);

		iprintf(" %d x %d using %d palettes\n", (int) newAsset->w, (int) newAsset->h, (int) numPalettes);

		// Seek past all those palette ids
		fseek(zbeData, numPalettes * sizeof(uint32), SEEK_CUR);

		// Get the size of the map data
		newAsset->length = load<uint32>(zbeData);
		iprintf(" %dB map data\n", newAsset->length);
		
		// Seek past all the map data
		fseek(zbeData, newAsset->length, SEEK_CUR);

		// Put this backgroundAsset on the vector
		backgroundAssets.push_back(newAsset);
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

		// The total number of bytes it takes to represent one level object in the
		// assets file.
		// NOTE: Keep this up to date!
		const static int lvlObjSize = 4 + 4 + 2 + 2;

		// number of level heroes
		uint32 numLvlHeroes = load<uint32>(zbeData);

		// Seek past those heroes
		fseek(zbeData, lvlObjSize * numLvlHeroes, SEEK_CUR);

		// number of level objects
		uint32 numLvlObjs = load<uint32>(zbeData);

		// Seek past all the level objects
		fseek(zbeData, lvlObjSize * numLvlObjs, SEEK_CUR);
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

	// background
	uint32 bg0id = load<uint32>(zbeData);
	lvl->bg0 = backgroundAssets[bg0id];

	// number of level heroes
	uint32 numLvlHeroes = load<uint32>(zbeData);
	iprintf(" #heroes %d\n", numLvlHeroes);

	// Allocate enough space for them all
	lvl->heroes = new levelObjectAsset*[numLvlHeroes + 1];
	lvl->heroes[numLvlHeroes] = NULL;

	// Load up all the heroes
	for (uint32 i = 0; i < numLvlHeroes; i++)
	{
		// load up the variables
		uint32 objId = load<uint32>(zbeData);
		uint16 x = load<uint16>(zbeData);
		uint16 y = load<uint16>(zbeData);
		iprintf("  #%d: obj%d at (%d, %d)\n", (int) i, (int) objId, (int) x, (int) y);

		// make a new levelObjectAsset and add it to the vector
		levelObjectAsset *lvlObj = new levelObjectAsset(vector2D<float>(float(x), float(y)), objectAssets[objId]);
		lvl->heroes[i] = lvlObj;
	}

	// number of level objects
	uint32 numLvlObjs = load<uint32>(zbeData);
	iprintf(" #objs %d\n", numLvlObjs);

	// Allocate enough space for all the level objects
	lvl->objects = new levelObjectAsset*[numLvlObjs + 1];
	lvl->objects[numLvlObjs] = NULL;

	// for each level object
	for (uint32 i = 0; i < numLvlObjs; i++)
	{
		// load relevant datas
		uint32 objId = load<uint32>(zbeData);
		uint16 x = load<uint16>(zbeData);
		uint16 y = load<uint16>(zbeData);
		iprintf("  #%d: obj%d at (%d, %d)\n", (int) i, (int) objId, (int) x, (int) y);

		// Make a new levelObjectAsset and add it to the vector
		levelObjectAsset *lvlObj = new levelObjectAsset(vector2D<float>(float(x), float(y)), objectAssets[objId]);
		lvl->objects[i] = lvlObj;
	}

	//close the file
	closeFile();

	// Return that levelAsset
	return lvl;
}


// loads up a background
int assets::loadBackground(backgroundAsset *background)
{
	// Return if nothing to do
	if (!background) return -1;

	iprintf("Loading background...\n");

	// TODO: make the backgroundAsset more like gfx since we need to keep the map data around
	//       might as well make it remember the other stuff too? hmm maybe not. 
	// Open the file
	openFile();

	// Seek to the proper place in the file
	if (fsetpos(zbeData, &(background->position)))
	{
		iprintf("Seek error: %s\n", strerror(errno));
		die();
	}

	// Init the background
	// TODO: un-hard-code this layer value here
	// bgId = bgInit(layer, bgType, bgSize, mapBase, tileBase)
	int bgId = bgInit(0, BgType_Text4bpp,  ZOIDBERG_BACKGROUND_SIZE, 0, 2);
	bgSetPriority(bgId, 0);

	// Get number of palettes to load
	uint8 numPalettes = load<uint8>(zbeData);
	iprintf(" %d palettes\n", numPalettes);

	// Load up all the palettes into a vector
	uint16 offset = 0;
	vector<uint32> paletteIds;
	for (uint8 i = 0; i < numPalettes; i++)
	{
		// Get palette id
		uint32 palId = load<uint32>(zbeData);
		iprintf("  %d -> %d\n", i, palId);

		paletteIds.push_back(palId);
	}

	// If the background's map isn't loaded yet, load it
	if (!background->mmLoaded)
	{
		// Get the length of the map data
		background->length = load<uint32>(zbeData);
		iprintf(" Loading %dB map\n", background->length);

		// Allocate some space for that data and load it up
		background->data = (uint16*) malloc(background->length * sizeof(uint8));
		if (fread(background->data, sizeof(uint8), background->length, zbeData) < background->length)
		{
			iprintf("Error reading background map from file: %s\n", strerror(errno));
			die();
		}
		
		// Indicate that the background is mmLoaded
		background->mmLoaded = true;
	}
	
	closeFile();

	// Make sure tiles data is loaded then copy it into video memory
	gfxAsset *tileset = background->tileset;
	loadGfx(tileset);
	iprintf(" ld %dB, cp tileset (%x)\n", tileset->length, (unsigned int) bgGetGfxPtr(bgId));
	DC_FlushRange(tileset->data, tileset->length);
	dmaCopy(tileset->data, bgGetGfxPtr(bgId), tileset->length);
	// unload the tileset
	freeGfx(background->tileset);

	// Copy the visible part of the background into video memory
	iprintf("  load map\n");
	DC_FlushRange(background->data, background->length);
	uint16 *mapPtr = bgGetMapPtr(bgId);
	
	// Row Major Order
	// We need to copy enough tiles to fill the height of the screen plus some buffer
	for (uint32 y = 0; y < background->h; y++)
	{
		for (uint32 x = 0; x < background->w; x++)
		{
			// TODO: MOVE ALL REFERENCES TO ASSET SIZES INTO #DEFINES SO THAT CHANGING THEM IS VERY EASY.
			// Copy this tile of the map                                                      Each tile is 2 bytes \/
			uint32 mapOffset = (y * background->w) + x;
			uint32 bgOffset = (y * ZOIDBERG_BACKGROUND_TILE_WIDTH) / ZOIDBERG_BACKGROUND_BYTES_PER_TILE + x;
			dmaCopy(background->data + mapOffset, mapPtr + bgOffset, sizeof(uint16));
		}
		iprintf("\n");
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

	// Change the priority
	bgSetPriority(bgId, 3);

	return bgId;
}


// Loads a gfx into main memory
void assets::loadGfx(gfxAsset *gfx)
{
	// If passed NULL or the gfx is already in main memory, return
	if (!gfx || gfx->mmLoaded) return;

	// Need to load the gfx from the file and check for errors
	openFile();

	// Need to load it from disk into memory
	// Seek to the proper place in the file
	if (fsetpos(zbeData, &(gfx->position)))
	{
		iprintf("Seek error: %s\n", strerror(errno));
		die();
	}

	// Load up the data
	gfx->data = (uint16*) malloc(gfx->length * sizeof(uint8));
	if (fread(gfx->data, sizeof(uint8), gfx->length, zbeData) < gfx->length)
	{
		iprintf("Error reading gfx from file:\n  %s", strerror(errno));
		die();
	}

	iprintf("gfx mmLoaded -> %x\n", (unsigned int) gfx->data);

	// Everything is A-Okay! close the file and set the gfx to mmLoaded
	closeFile();
	gfx->mmLoaded = true;
}


// Free mm space used by gfxAsset
void assets::freeGfx(gfxAsset *gfx)
{
	if(!gfx || !gfx->mmLoaded) return;

	// Free memory
	delete gfx->data;
	// Reset variable
	gfx->mmLoaded = false;
}


// returns the offset for the gfxAsset in video memory. DMA copies if needed
uint16 *assets::getGfx(gfxAsset *gfx)
{
	// Don't do anything if passed a NULL pointer
	if (!gfx)
		return 0;

	// If it's already loaded, just return its offset
	if (gfx->vmLoaded)
		return gfx->offset;

	// If the gfx isn't in main memory (which it should be), run loadGfx()
	if (!gfx->mmLoaded)
		loadGfx(gfx);

	// It would appear that the gfx is in main memory, but video memory.
	// So load it up!

	// Allocate video memory for the gfx
	uint16 *mem = oamAllocateGfx(oam, gfx->size, SpriteColorFormat_16Color);
	gfx->offset = mem;
	gfx->vmLoaded = true;

	// Start copying asynchronously
	uint16 length = gfx->length;
	DC_FlushRange(gfx->data, length);
	dmaCopyHalfWordsAsynch(3, gfx->data, mem, length);

	iprintf("gfx vmLoaded -> %x\n", (unsigned int) gfx->offset);

	return mem;
}


// Loads a palette off of the disk and into main memory
void assets::loadPalette(paletteAsset *pal)
{
	// If passed NULL or if it's already loaded, return
	if (!pal || pal->mmLoaded) return;

	// Need to load the gfx from the file
	openFile();

	// Seek to the proper place in the file
	if (fsetpos(zbeData, &(pal->position)))
	{
		iprintf("Seek error: %s\n", strerror(errno));
		die();
	}

	// Load into main memory. If it fails, die
	uint16 length = pal->length;
	pal->data = (uint16*) malloc(length * sizeof(u8));
	if (fread(pal->data, sizeof(u8), length, zbeData) < length)
	{
		iprintf("Error reading palette from file:\n  %s", strerror(errno));
		die();
	}

	iprintf("pal mmLoaded -> %x\n", (unsigned int) pal->data);

	// All done, close the file and set mmLoaded
	closeFile();
	pal->mmLoaded = true;
}


// Free mm space used for paletteAsset
void assets::freePalette(paletteAsset *pal)
{
	if (!pal || !pal->mmLoaded) return;

	// Free and reset
	delete pal->data;
	pal->mmLoaded = false;
}


// Returns index of passed paletteAsset in video memory. DMA copies it if needed
uint8 assets::getPalette(paletteAsset *pal)
{
	// Don't do anything if passed NULL
	if (!pal) return 0;

	// Return its index if it's already loaded
	if (pal->vmLoaded)
		return pal->index;

	// If it's not in main memory for some reason, load it
	if (!pal->mmLoaded)
		loadPalette(pal);

	// Keep track of which index a palette can be loaded into
	static int curIndex = 0;

	// TODO: add support for 256 color palettes
	static const int numColors = 16;

	DC_FlushRange(pal->data, pal->length);
	dmaCopyHalfWordsAsynch(3, pal->data, &SPRITE_PALETTE[curIndex * numColors], pal->length);

	// Update the paletteAsset's information
	pal->vmLoaded = true;
	pal->index = curIndex;
	iprintf("pal vmLoaded -> %d\n", curIndex);

	// Update the index for the next call
	curIndex++;

	// return index
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
		sheight = 64;

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
