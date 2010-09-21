#include "assets.h"

assets::assets(char* input, OamState *table)
{
	// Set variables
	oam = table;
	zbeFile = input;
	lastLevel = NULL;

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

	// Check to see if it has the testing flag
#ifndef ZBE_TESTING
	if (version & 1 << 15)
	{
		iprintf("Error: Attempting to open a\nTesting ZBE file without\ntesting enabled.\n");
		die();
	}

	// Check to see if it's the supported version
	if (version != ZBE_VERSION_SUPPORTED)
#else
	if (version != (ZBE_VERSION_SUPPORTED | 1 << 15))
#endif
	{
		iprintf("Error: Attempting to open a\nZBE file with an unsupported\nversion\n");
		die();
	}

	// Clear the testing flag and print the version
	iprintf("v %d", (version & 0x7FFF) );
#ifdef ZBE_TESTING
	iprintf(" with testing flag\n");
#else
	iprintf("\n");
#endif

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




	// pause if we're testing
	pauseIfTesting();




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



	// pause if we're testing
	pauseIfTesting();




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




	// pause if we're testing
	pauseIfTesting();




	// Number of backrounds
	uint32 numBackgrounds = load<uint32>(zbeData);
	iprintf("#bgs %d\n", numBackgrounds);

	// Get all the backgrounds
	for (unsigned int i = 0; i < numBackgrounds; i++)
	{
		// make a new backgroundAsset
		backgroundAsset *newAsset = new backgroundAsset();

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





	// pause if we're testing
	pauseIfTesting();





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




	// pause if we're testing
	pauseIfTesting();




	// Number of levels
	uint32 numLvls = load<uint32>(zbeData);
	iprintf("#lvls %d\n", numLvls);

	// for each level
	for (uint32 i = 0; i < numLvls; i++)
	{
		// Make a new levelAsset
		levelAsset *newAsset = new levelAsset;
		newAsset->objects = NULL;

		// Get the level name's length and allocate space for the string
		uint32 nameLen = load<uint32>(zbeData);
		if (nameLen > 0)
		{
			newAsset->name = new char[nameLen + 1];
			// Load up the name string
			for (unsigned int c = 0; c < nameLen; c++)
				newAsset->name[c] = (char) load<uint8>(zbeData);
			newAsset->name[nameLen] = '\0';
		}
		iprintf(" %d: %s\n", i, newAsset->name);

		// Get the level's dimensions

		// Set the location variable
		fpos_t curPos;
		fgetpos(zbeData, &curPos);
		newAsset->position = curPos;

		// Push this asset onto the levelAssets vector
		levelAssets.push_back(newAsset);

		// Skip over the level's dimensions
		load<uint32>(zbeData);
		load<uint32>(zbeData);

#ifdef ZBE_TESTING
		// Skip over the test explanation message
		uint32 expLen = load<uint32>(zbeData);
		fseek(zbeData, expLen * sizeof(uint8), SEEK_CUR);

		// Skip over the debug explanation
		uint32 dbgLen = load<uint32>(zbeData);
		fseek(zbeData, dbgLen * sizeof(uint8), SEEK_CUR);
		iprintf(" exp %d chars, dbg %d chars\n", (int) expLen, (int) dbgLen);

		// Skip the timer
		uint16 timer = load<uint16>(zbeData);
		iprintf(" %d timer", (int) timer);
#endif

		// Skip over the backgrounds
		// NOTE: keep this up to date!
		//                        bg0     bg1     bg2     bg3     tileset
		const static int bgSize = 4 + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 4;
		fseek(zbeData, bgSize, SEEK_CUR);
		iprintf(" bgs\n");
		
		// Skip over the platforms
		// NOTE: Keep this up to date!
		//                          X   Y   i   j
		const static int platSize = 2 + 2 + 2 + 2;
		uint32 numLvlPlatforms = load<uint32>(zbeData);
		load<uint32>(zbeData); // Number vector platforms
		load<uint32>(zbeData);  // number rectangle platforms
		fseek(zbeData, platSize * numLvlPlatforms, SEEK_CUR);
		iprintf(" %d platforms", numLvlPlatforms);

		// The total number of bytes it takes to represent one level object in the
		// assets file.
		// NOTE: Keep this up to date!
		//                            object Id  X   Y   hgrav vgrav
		const static int lvlObjSize = 4 +        2 + 2 + 4 +   4;

		// number of level heroes
		uint32 numLvlHeroes = load<uint32>(zbeData);

		// Seek past those heroes
		fseek(zbeData, lvlObjSize * numLvlHeroes, SEEK_CUR);
		iprintf(" %d heroes", numLvlHeroes);

		// number of level objects
		uint32 numLvlObjs = load<uint32>(zbeData);

		// Seek past all the level objects
		fseek(zbeData, lvlObjSize * numLvlObjs, SEEK_CUR);
		iprintf(" %d objs\n", numLvlObjs);
	}

	closeFile();
}


// Deconstructor
assets::~assets()
{
	for (unsigned int i = 0; i < gfxAssets.size(); i++)
	{
		// Free the video memory it may be using
		if (gfxAssets[i]->vmLoaded)
			oamFreeGfx(oam, gfxAssets[i]->offset);

		delete gfxAssets[i];
	}
	for (unsigned int i = 0; i < tilesetAssets.size(); i++)
		delete tilesetAssets[i];
	for (unsigned int i = 0; i < paletteAssets.size(); i++)
		delete paletteAssets[i];
	for (unsigned int i = 0; i < backgroundAssets.size(); i++)
		delete backgroundAssets[i];
	for (unsigned int i = 0; i < objectAssets.size(); i++)
		delete objectAssets[i];
	for (unsigned int i = 0; i < levelAssets.size(); i++)
		delete levelAssets[i];
}


// Load and return a level's metadata
levelAsset *assets::loadLevel(uint32 id)
{
	// open the file
	openFile();

	// keep track of the last one opened
	levelAsset *lvl = lastLevel;

	// Clear out the last one if this isn't the first time
	if (lvl)
	{
		lvl->clear();
		lvl = NULL;
	}

	// Bounds checking
	if (id >= levelAssets.size())
	{
		iprintf("ERROR: Level id is out of bounds.\n       Requested %d, max %d", id, levelAssets.size() - 1);
		die();
	}

	// Update last
	lvl = levelAssets[id];

	// Seek to the proper place in the file
	fsetpos(zbeData, &(lvl->position));
	iprintf("lvl %d requested\n", id);

	// Load up the level's dimensions
	lvl->dimensions.x = load<uint32>(zbeData);
	lvl->dimensions.y = load<uint32>(zbeData);

#ifdef ZBE_TESTING
	// Test explanation message
	uint32 expLen = load<uint32>(zbeData);
	if (expLen > 0)
	{
		lvl->expMessage = new char[expLen + 1];
		for (unsigned int i = 0; i < expLen; i++)
			lvl->expMessage[i] = (char) load<uint8>(zbeData);
		lvl->expMessage[expLen] = '\0';
	}

	// Debug explanation message
	uint32 dbgLen = load<uint32>(zbeData);
	if (dbgLen > 0)
	{
		lvl->debugMessage = new char[dbgLen + 1];
		for (unsigned int i = 0; i < dbgLen; i++)
			lvl->debugMessage[i] = (char) load<uint8>(zbeData);
		lvl->debugMessage[dbgLen] = '\0';
	}

	// Timer value
	lvl->timer = load<uint16>(zbeData);
#endif

	// background -1 means it was not set
	// Load up all the backgrounds
	for (int i = 0; i < 4; i++)
	{
		// Get the id and distance
		uint32 bgId = load<uint32>(zbeData);
		uint8 dist = load<uint8>(zbeData);

		// background -1 means it was not set, so make that bg's pointer to the backgroundAsset NULL in that case
		lvl->bgs[i].background = (bgId == uint32(-1)) ? NULL : backgroundAssets[bgId];
		lvl->bgs[i].distance = dist;
		lvl->bgs[i].layer = i;

		iprintf("  bg%d: bg%d at %d\n", i, bgId, dist);
	}
	// Set the tileset to use
	uint32 tilesetId = load<uint32>(zbeData);
	lvl->tileset = tilesetAssets[tilesetId];
	iprintf(" using tileset %d for bgs\n", tilesetId);
	
	
	
	// number of platforms
	uint32 totNumLvlPlatforms = load<uint32>(zbeData);
	uint32 numLvlVecPlatforms = load<uint32>(zbeData);
	uint32 numLvlSqPlatforms = load<uint32>(zbeData);
	iprintf(" #plats %d + %d = %d\n", numLvlVecPlatforms, numLvlSqPlatforms, totNumLvlPlatforms);
	
	// allocate memory
	lvl->platforms = new levelPlatformAsset*[totNumLvlPlatforms + 1];
	lvl->platforms[totNumLvlPlatforms + 1] = NULL;
	
	// Load them, Vector Platforms
	for (uint32 n = 0; n < numLvlVecPlatforms; n++)
	{
		uint16 x = load<uint16>(zbeData);
		uint16 y = load<uint16>(zbeData);
		int16 i = load<int16>(zbeData);
		int16 j = load<int16>(zbeData);
		
		iprintf("  #%d: vec (%d, %d) <%d, %d>\n", n, x, y, i , j);
		
		// create the platform
		levelPlatformAsset *lvlPlat = new levelPlatformAsset(ZBE_PLATFORM_TYPE_VEC, vector2D<float>(float(x), float(y)), vector2D<float>(float(x), float(y)));
		lvl->platforms[n] = lvlPlat;
	}
	// Square Platforms
	for (uint32 n = numLvlVecPlatforms; n < numLvlVecPlatforms + numLvlSqPlatforms; n++)
	{
		uint16 x = load<uint16>(zbeData);
		uint16 y = load<uint16>(zbeData);
		uint16 i = load<uint16>(zbeData);
		uint16 j = load<uint16>(zbeData);
		
		iprintf("  #%d: sq (%d, %d) %d x %d\n", n, x, y, i , j);
		
		// create the platform
		levelPlatformAsset *lvlPlat = new levelPlatformAsset(ZBE_PLATFORM_TYPE_RECT, vector2D<float>(float(x), float(y)), vector2D<float>(float(x), float(y)));
		lvl->platforms[n] = lvlPlat;
	}
			
		
	

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
		int32 ihgrav = load<int32>(zbeData);
		int32 ivgrav = load<int32>(zbeData);
		// Convert 12.20 ints into floats by dividing by 2^12 = 4096
		float fhgrav = float(ihgrav) / 4096.0;
		float fvgrav = float(ivgrav) / 4096.0;

		iprintf("  #%d: obj%d at (%d, %d)\n", (int) i, (int) objId, (int) x, (int) y);
		printf("       grav (%f, %f)\n", fhgrav, fvgrav);

		// make a new levelObjectAsset and add it to the vector
		levelObjectAsset *lvlObj = new levelObjectAsset(vector2D<float>(float(x), float(y)), vector2D<float>(fhgrav, fvgrav), objectAssets[objId]);
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
		int32 ihgrav = load<int32>(zbeData);
		int32 ivgrav = load<int32>(zbeData);
		// Convert 12.20 ints into floats by dividing by 2^12 = 4096
		float fhgrav = float(ihgrav) / 4096.0;
		float fvgrav = float(ivgrav) / 4096.0;

		iprintf("  #%d: obj%d at (%d, %d)\n", (int) i, (int) objId, (int) x, (int) y);
		printf("       grav (%f, %f)\n", fhgrav, fvgrav);

		// Make a new levelObjectAsset and add it to the vector
		levelObjectAsset *lvlObj = new levelObjectAsset(vector2D<float>(float(x), float(y)), vector2D<float>(fhgrav, fvgrav), objectAssets[objId]);
		lvl->objects[i] = lvlObj;
	}

	//close the file
	closeFile();

	// Return that levelAsset
	return lvl;
}


// loads up a background
void assets::loadBackground(levelBackgroundAsset *lvlBackground)
{
	// Return if nothing to do
	if (!lvlBackground) return;

	iprintf("Loading background...\n");
	backgroundAsset *background = lvlBackground->background;

	// Open the file
	openFile();

	// Seek to the proper place in the file
	if (fsetpos(zbeData, &(background->position)))
	{
		iprintf("Seek error: %s\n", strerror(errno));
		die();
	}

	// Get number of palettes to load
	uint8 numPalettes = load<uint8>(zbeData);
	iprintf(" %d palettes\n", numPalettes);

	// Load up all the palettes into a vector
	for (uint8 i = 0; i < numPalettes; i++)
	{
		// Get palette id
		uint32 palId = load<uint32>(zbeData);
		iprintf("  %d -> %d\n", i, palId);

		lvlBackground->palettes.push_back(paletteAssets[palId]);
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

	//iprintf("gfx mmLoaded -> %x\n", (unsigned int) gfx->data);

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

	//iprintf("gfx vmLoaded -> %x\n", (unsigned int) gfx->offset);

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

	//iprintf("pal mmLoaded -> %x\n", (unsigned int) pal->data);

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
	//iprintf("pal vmLoaded -> %d\n", curIndex);

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
