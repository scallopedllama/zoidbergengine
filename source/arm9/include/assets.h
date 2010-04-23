/**
 * @file assets.h
 *
 * @brief The assets class manages all assets stored in the zbe datafile for a game
 *
 * This file contains the assets class. The assets class manages the current game's
 * zoidberg engine zbe datafile. It is in charge of loading all necessary graphical
 * data from the disk into the DS's memory and keep track of what data can be removed
 * from memory. It tracks this information to allow a game to use as many different
 * sprites as possible without running out of memory.
 * Upon initialization, the assets class scans through the data file looking for markers
 * that indicate where data begins. It notes these locations in arrays for their relevant
 * datatypes. For example, it has a vector of integers called villanLocations where each
 * index indicates where to seek in the datafile to load the graphical data for the villan
 * with that index.
 * When a level is initialized, it asks the assets class to make sure that all
 * the graphical data needed for that level is loaded into memory. The assets class
 * then loads the vectors of all the objects used in that level for the level class.
 * The level then uses those vectors to update all objects.
 *
 * @author Joe Balough
 */

/**
 *  Copyright (c) 2010 zoidberg engine
 *
 *  This file is part of the zoidberg engine.
 *
 *  The zoidberg engine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The zoidberg engine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the zoidberg engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ASSETS_H_INCLUDED
#define ASSETS_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <nds.h>
#include <fat.h>
#include <vector>
#include "vector.h"
#include "assettypes.h"
#include "util.h" // die()

using namespace std;


/**
 * The assets class, manages all assets that are stored in the zbe datafile
 *
 * @author Joe Balough
 */
class assets {
public:
	/**
	 * @param string filename
	 *   The zbe file to use for this game
	 * @param OamState *oam
	 *   The oam table into which the graphics should be loaded
	 * @author Joe Balough
	 */
	assets(char* filename, OamState *oam);

	/**
	 * parseZbe function
	 *
	 * Parses a zbe file and notes indices for loading. Loads only the lengths into memory.
	 * All actual data (tiles, palettes, etc.) are loaded upon request.
	 *
	 * @author Joe Balough
	 */
	// TODO: update the docs for this too
	void parseZbe();

	/**
	 * getGfx function
	 *
	 * Returns a pointer to the location in video memory where the tiles for the passed gfxAsset
	 * can be found. Will copy those tiles into video memory if needed.
	 *
	 * @param gfxAsset *gfx
	 *   the gfxAsset to load
	 * @return uint16 pointer
	 *   a pointer to the location in video memory into which the gfx were loaded
	 * @author Joe Balough
	 */
	uint16 *getGfx(gfxAsset *gfx);

	/**
	 * getPalette function
	 *
	 * Returns the index to the location in video memory where the palette data for the passed paletteAsset
	 * can be found. Will copy that palette into video memory if needed.
	 *
	 * @param palAsset *pal
	 *   A pointer to the palAsset of the palette to load
	 * @return uint8
	 *   The index of the loaded palette
	 * @author Joe Balough
	 */
	uint8 getPalette(paletteAsset *pal);

	/**
	 * loadLevel function
	 *
	 * Used by the level class to get the metadata for a level. This function will free up any
	 * memory used by the last loaded levelAsset object then parse the zbeData file to load up
	 * the relevant vectors and return the levelAsset object.
	 *
	 * @param uint32 id
	 *   id for the level to load
	 * @return levelAsset*
	 *   A pointer to the level's metadata to be used by the level class to load up objects and the like
	 * @author Joe Balough
	 */
	levelAsset *loadLevel(uint32 id);

	/**
	 * loadBackground function
	 *
	 * Used by background class to tell this assets class to initialize the passed backgroundAsset
	 * and get its data into main memory properly.
	 *
	 * @param backgroundAsset *background
	 *  The backgroundAsset for the background to load. Obtained from a levelAsset
	 * @author Joe Balough
	 */
	void loadBackground(backgroundAsset *background);

	/**
	 * Retrieve the SpriteSize for the gfx with the specified id
	 * @param uint32 id
	 *  Id for the gfx whose size is desired
	 * @author Joe Baough
	 */
	inline SpriteSize getSpriteSize(uint32 id)
	{
		return gfxAssets[id]->size;
	}

private:
	/**
	 * fread wrapper; load
	 *
	 * the load function reads a number of the specified type off of the
	 * passed input FILE and return it.
	 *
	 * @param FILE *input
	 *   The file from which to read the number
	 * @return T
	 *   The value read from the file
	 * @author Joe Balough
	 */
	template <class T> T load(FILE *input);


	/**
	 * loadGfx() function
	 *
	 * Loads the passed gfxAsset into main memory. will be copied into video memory at the first
	 * call to getGfx()
	 *
	 * @author Joe Balough
	 */
	void loadGfx(gfxAsset *gfx);


	/**
	 * freeGfx() function
	 *
	 * Frees the space allocated to hold the passed gfxAsset in main memory. Resets the loaded variable.
	 *
	 * @author Joe Balough
	 */
	void freeGfx(gfxAsset *gfx);


	/**
	 * loadPalette() function
	 *
	 * Loads the passed paletteAsset into main memory. will be copied into video memory at the first
	 * call to getPalette()
	 *
	 * @author Joe Balough
	 */
	void loadPalette(paletteAsset *gfx);


	/**
	 * freePalette() function
	 *
	 * Frees the space allocated to hold the passed paletteAsset in main memory. Resets the loaded variable.
	 *
	 * @author Joe Balough
	 */
	void freePalette(paletteAsset *gfx);


	/**
	 * openFile function
	 *
	 * Opens the zbeData file for reading. All functions parsing data from the file needs
	 * to call this before attempting any reading and should call closeFile() before returning.
	 *
	 * @author Joe Balough
	 */
	inline void openFile()
	{
		// Try to open the file. If it fails, print something and die.
		zbeData = fopen(zbeFile, "rb");
		if (!zbeData)
		{
			iprintf("Error opening datafile %s: %s\n", zbeFile, strerror(errno));
			die();
		}
	}

	/**
	 * closefile function
	 *
	 * Closes the zbeData file previously opened with openFile. Should be called before
	 * returning on any function that parses data from the zbeData.
	 *
	 * @author Joe Balough
	 */
	inline void closeFile()
	{
		// Don't try to close it if it's not open. if it's open, close the file and set the pointer to NULL.
		if(zbeData)
		{
			fclose(zbeData);
			zbeData = NULL;
		}
	}

	/**
	 * getSpriteSize function
	 *
	 * getSpriteSize is simply a lengthy string of switch cases to find the appropriate
	 * SpriteSize to fit the passed width and height. It's used in the parsing function.
	 * If a width or height has an unexpected value, it will give it the largest size in
	 * that undefined dimension.
	 *
	 * @param uint8 width
	 * @param uint8 height
	 *   dimensions of the sprite's graphics data
	 * @author Joe Balough
	 */
	SpriteSize getSpriteSize(uint8 width, uint8 height);

	// The zbe file
	FILE *zbeData;

	// zbe Filename
	char *zbeFile;

	// A pointer to the oamState
	OamState *oam;

	/**
	 * These vectors correspond to the status of the assets. They indicate whether or not the
	 * id asset are loaded, their index if loaded, the position in the file, length, size, etc.
	 * @see assetStatus
	 */
	vector<gfxAsset*> gfxAssets;
	vector<paletteAsset*> paletteAssets;

	vector<gfxAsset*> tilesetAssets;
	vector<backgroundAsset*> backgroundAssets;

	vector<objectAsset*> objectAssets;
	vector<levelAsset*> levelAssets;
};

#endif // ASSETS_H_INCLUDED
